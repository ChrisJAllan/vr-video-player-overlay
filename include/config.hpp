#pragma once

#include <string>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include <pwd.h>
#include <sys/stat.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

struct SphereConfig {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    float zoom = 0.0f;
};

struct Sphere360Config {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    float zoom = 0.0f;
};

struct FlatConfig {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    float zoom = 0.0f;
};

struct PlaneConfig {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    float zoom = 0.0f;
};

struct Config {
    SphereConfig sphere;
    Sphere360Config sphere360;
    FlatConfig flat;
    PlaneConfig plane;
};

static std::string get_home_dir() {
    const char *home_dir = getenv("HOME");
    if(!home_dir) {
        passwd *pw = getpwuid(getuid());
        home_dir = pw->pw_dir;
    }

    if(!home_dir) {
        fprintf(stderr, "Error: Failed to get home directory of user, using /tmp directory\n");
        home_dir = "/tmp";
    }

    return home_dir;
}

static std::string get_config_dir() {
    std::string config_dir;
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    if(xdg_config_home) {
        config_dir = xdg_config_home;
    } else {
        config_dir = get_home_dir() + "/.config";
    }
    config_dir += "/vr-video-player";
    return config_dir;
}

static int create_directory_recursive(char *path) {
    int path_len = strlen(path);
    char *p = path;
    char *end = path + path_len;
    for(;;) {
        char *slash_p = strchr(p, '/');

        // Skips first '/', we don't want to try and create the root directory
        if(slash_p == path) {
            ++p;
            continue;
        }

        if(!slash_p)
            slash_p = end;

        char prev_char = *slash_p;
        *slash_p = '\0';
        int err = mkdir(path, S_IRWXU);
        *slash_p = prev_char;

        if(err == -1 && errno != EEXIST)
            return err;

        if(slash_p == end)
            break;
        else
            p = slash_p + 1;
    }
    return 0;
}

static bool file_get_content(const char *filepath, std::string &file_content) {
    file_content.clear();
    bool success = false;

    FILE *file = fopen(filepath, "rb");
    if(!file)
        return success;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if(file_size != -1) {
        file_content.resize(file_size);
        fseek(file, 0, SEEK_SET);
        if((long)fread(&file_content[0], 1, file_size, file) == file_size)
            success = true;
    }

    fclose(file);
    return success;
}

struct StringView {
    const char *str;
    size_t size;

    bool operator == (const char *other) const {
        int len = strlen(other);
        return (size_t)len == size && memcmp(str, other, size) == 0;
    }

    size_t find(char c) const {
        const void *p = memchr(str, c, size);
        if(!p)
            return std::string::npos;
        return (const char*)p - str;
    }
};

using StringSplitCallback = std::function<bool(StringView line)>;

static void string_split_char(const std::string &str, char delimiter, StringSplitCallback callback_func) {
    size_t index = 0;
    while(index < str.size()) {
        size_t new_index = str.find(delimiter, index);
        if(new_index == std::string::npos)
            new_index = str.size();

        if(!callback_func({str.data() + index, new_index - index}))
            break;

        index = new_index + 1;
    }
}

static bool config_split_key_value(const StringView str, StringView &key, StringView &value) {
    key.str = nullptr;
    key.size = 0;

    value.str = nullptr;
    value.size = 0;

    size_t index = str.find(' ');
    if(index == std::string::npos)
        return std::string::npos;

    key.str = str.str;
    key.size = index;

    value.str = str.str + index + 1;
    value.size = str.size - (index + 1);
    
    return true;
}

static bool string_to_float(std::string str, float &value, const StringView key) {
    value = 0.0;
    errno = 0;
    char *endptr;
    value = strtod(str.c_str(), &endptr);
    if(endptr == str.c_str() || errno != 0) {
        fprintf(stderr, "Warning: Invalid config option %.*s is not a float\n", (int)key.size, key.str);
        return false;
    }
    return true;
}

static bool string_to_vec3(std::string str, glm::vec3 &value, const StringView key) {
    if(sscanf(str.data(), "%f|%f|%f", &value.x, &value.y, &value.z) != 3) {
        fprintf(stderr, "Warning: Invalid config option %.*s is not a glm::vec3\n", (int)key.size, key.str);
        return false;
    }
    return true;
}

static bool string_to_quat(std::string str, glm::quat &value, const StringView key) {
    if(sscanf(str.data(), "%f|%f|%f|%f", &value.x, &value.y, &value.z, &value.w) != 4) {
        fprintf(stderr, "Warning: Invalid config option %.*s is not a glm::quat\n", (int)key.size, key.str);
        return false;
    }
    return true;
}

static Config read_config(bool &exists) {
    setlocale(LC_ALL, "C");

    Config config;

    const std::string config_path = get_config_dir() + "/config";
    std::string file_content;
    if(!file_get_content(config_path.c_str(), file_content)) {
        fprintf(stderr, "Warning: Failed to read config file: %s\n", config_path.c_str());
        exists = false;
        return config;
    }

    string_split_char(file_content, '\n', [&](StringView line) {
        StringView key, value;
        if(!config_split_key_value(line, key, value)) {
            fprintf(stderr, "Warning: Invalid config option format: %.*s\n", (int)line.size, line.str);
            return true;
        }

        if(key == "sphere.position") {
            string_to_vec3(std::string(value.str, value.size), config.sphere.position, key);
        } else if(key == "sphere.rotation") {
            string_to_quat(std::string(value.str, value.size), config.sphere.rotation, key);
        } else if(key == "sphere.zoom") {
            string_to_float(std::string(value.str, value.size), config.sphere.zoom, key);
        } else if(key == "sphere360.position") {
            string_to_vec3(std::string(value.str, value.size), config.sphere360.position, key);
        } else if(key == "sphere360.rotation") {
            string_to_quat(std::string(value.str, value.size), config.sphere360.rotation, key);
        } else if(key == "sphere360.zoom") {
            string_to_float(std::string(value.str, value.size), config.sphere360.zoom, key);
        } else if(key == "flat.position") {
            string_to_vec3(std::string(value.str, value.size), config.flat.position, key);
        } else if(key == "flat.rotation") {
            string_to_quat(std::string(value.str, value.size), config.flat.rotation, key);
        } else if(key == "flat.zoom") {
            string_to_float(std::string(value.str, value.size), config.flat.zoom, key);
        } else if(key == "plane.position") {
            string_to_vec3(std::string(value.str, value.size), config.plane.position, key);
        } else if(key == "plane.rotation") {
            string_to_quat(std::string(value.str, value.size), config.plane.rotation, key);
        } else if(key == "plane.zoom") {
            string_to_float(std::string(value.str, value.size), config.plane.zoom, key);
        } else {
            fprintf(stderr, "Warning: Invalid config option: %.*s\n", (int)line.size, line.str);
        }

        return true;
    });

    exists = true;
    return config;
}

static void save_config(const Config &config) {
    setlocale(LC_ALL, "C");

    const std::string config_path = get_config_dir() + "/config";

    char dir_tmp[PATH_MAX];
    strcpy(dir_tmp, config_path.c_str());
    char *dir = dirname(dir_tmp);

    if(create_directory_recursive(dir) != 0) {
        fprintf(stderr, "Warning: Failed to create config directory: %s\n", dir);
        return;
    }

    FILE *file = fopen(config_path.c_str(), "wb");
    if(!file) {
        fprintf(stderr, "Warning: Failed to create config file: %s\n", config_path.c_str());
        return;
    }

    fprintf(file, "sphere.position %f|%f|%f\n", config.sphere.position.x, config.sphere.position.y, config.sphere.position.z);
    fprintf(file, "sphere.rotation %f|%f|%f|%f\n", config.sphere.rotation.x, config.sphere.rotation.y, config.sphere.rotation.z, config.sphere.rotation.w);
    fprintf(file, "sphere.zoom %f\n", config.sphere.zoom);

    fprintf(file, "sphere360.position %f|%f|%f\n", config.sphere360.position.x, config.sphere360.position.y, config.sphere360.position.z);
    fprintf(file, "sphere360.rotation %f|%f|%f|%f\n", config.sphere360.rotation.x, config.sphere360.rotation.y, config.sphere360.rotation.z, config.sphere360.rotation.w);
    fprintf(file, "sphere360.zoom %f\n", config.sphere360.zoom);

    fprintf(file, "flat.position %f|%f|%f\n", config.flat.position.x, config.flat.position.y, config.flat.position.z);
    fprintf(file, "flat.rotation %f|%f|%f|%f\n", config.flat.rotation.x, config.flat.rotation.y, config.flat.rotation.z, config.flat.rotation.w);
    fprintf(file, "flat.zoom %f\n", config.flat.zoom);

    fprintf(file, "plane.position %f|%f|%f\n", config.plane.position.x, config.plane.position.y, config.plane.position.z);
    fprintf(file, "plane.rotation %f|%f|%f|%f\n", config.plane.rotation.x, config.plane.rotation.y, config.plane.rotation.z, config.plane.rotation.w);
    fprintf(file, "plane.zoom %f\n", config.plane.zoom);

    fclose(file);
}