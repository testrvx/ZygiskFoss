#include <mntent.h>
#include <sys/mount.h>

#include "files.hpp"
#include "logging.h"
#include "misc.hpp"
#include "zygisk.hpp"

#include "string.hpp"

#include <mimalloc-override.h>

using namespace std::string_view_literals;

namespace {
    constexpr auto MODULE_DIR = "/data/adb/modules";
    constexpr auto KSU_OVERLAY_SOURCE = "KSU";
    constexpr auto APATCH_OVERLAY_SOURCE = "APatch";
    constexpr auto ZYGISK_FUSE_SOURCE = "zygisk";

    void lazy_unmount(const char* mountpoint) {
        if (umount2(mountpoint, MNT_DETACH) != -1) {
            LOGD("Unmounted (%s)", mountpoint);
        } else {
#ifndef NDEBUG
            PLOGE("Unmount (%s)", mountpoint);
#endif
        }
    }
}

void revert_unmount_ksu() {
    sdstring ksu_loop;
    std::vector<sdstring> targets;

    targets.emplace_back(MODULE_DIR);

    for (auto& info: parse_mount_info("self")) {
        if (info.target == MODULE_DIR) {
            ksu_loop = info.source;
            continue;
        }
        // Unmount everything mounted to /data/adb
        if (info.target.starts_with("/data/adb")) {
            targets.emplace_back(info.target);
        }

        // unmount ksu overlays and temp dir
        if (info.type == "overlay" || info.type == "tmpfs") {
            if (info.source == KSU_OVERLAY_SOURCE) {
                targets.emplace_back(info.target);
            }
        }
        
       // Unmount /debug_ramdisk
        if (info.target.starts_with("/debug_ramdisk")) {
            targets.emplace_back(info.target);
        }
        // Unmount fuse
        if (info.type == "fuse" && info.source == ZYGISK_FUSE_SOURCE) {
            targets.emplace_back(info.target);
        }
  }
    for (auto& info: parse_mount_info("self")) {
        // Unmount everything from ksu loop except ksu module dir
        if (info.source == ksu_loop && info.target != MODULE_DIR) {
            targets.emplace_back(info.target);
        }
    }

    // Do unmount
    for (auto& s: reversed(targets)) {
        lazy_unmount(s.data());
    }
}

void revert_unmount_magisk() {
    std::vector<sdstring> targets;

    // Unmount dummy skeletons and MAGISKTMP
    // since mirror nodes are always mounted under skeleton, we don't have to specifically unmount
    for (auto& info: parse_mount_info("self")) {
        if (info.source == "magisk" || info.source == "worker" || // magisktmp tmpfs
            info.root.starts_with("/adb/modules")) { // bind mount from data partition
            targets.push_back(info.target);
        }
        // Unmount everything mounted to /data/adb
        if (info.target.starts_with("/data/adb")) {
            targets.emplace_back(info.target);
        }
    }

    for (auto& s: reversed(targets)) {
        lazy_unmount(s.data());
    }
}
void revert_unmount_apatch() {
    sdstring apatch_loop;
    std::vector<sdstring> targets;

    targets.emplace_back(MODULE_DIR);

    for (auto& info: parse_mount_info("self")) {
        if (info.target == MODULE_DIR) {
            apatch_loop = info.source;
            continue;
        } 
        // Unmount everything mounted to /data/adb
        if (info.target.starts_with("/data/adb")) {
            targets.emplace_back(info.target);
        }
        // Unmount APATCH overlays
        if (info.type == "overlay"
            && info.source == APATCH_OVERLAY_SOURCE) {
            targets.emplace_back(info.target);
        }
        // Unmount /debug_ramdisk
        if (info.target.starts_with("/debug_ramdisk")) {
            targets.emplace_back(info.target);
        }
        // Unmount fuse
        if (info.type == "fuse" && info.source == ZYGISK_FUSE_SOURCE) {
            targets.emplace_back(info.target);
        }
    }
    for (auto& info: parse_mount_info("self")) {
        // Unmount everything from apatch loop except apatch module dir
        if (info.source == apatch_loop && info.target != MODULE_DIR) {
            targets.emplace_back(info.target);
        }
    }

    // Do unmount
    for (auto& s: reversed(targets)) {
        lazy_unmount(s.data());
    }
}
