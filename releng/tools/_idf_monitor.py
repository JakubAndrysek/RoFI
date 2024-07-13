#!/usr/bin/env python3

# This module is just a thin wrapper around idf_monitor.py. It's goal is the
# only one - monkeypath the original implementation so it uses rmake instead of
# make

import importlib.util
import subprocess
import os
import sys
from esp_idf_monitor import idf_monitor

# Original implementation in the case it is needed
_run_make = idf_monitor.Monitor.run_make


def alternative_run_make(self, target):
    with self:
        target = target.replace("encrypted-", "")
        assert target in ["flash", "app-flash"]

        img = os.environ["ROFI_MONITORED_TARGET"]
        imgTarget = os.path.splitext(img)[0]
        toolsPath = os.path.join(os.environ["ROFI_ROOT"], "releng", "tools")

        try:
            subprocess.check_call(
                ["python", os.path.join(toolsPath, "rmake"), imgTarget]
            )
            subprocess.check_call(["python", os.path.join(toolsPath, "rflash"), img])
        except Exception as e:
            self.prompt_next_action("Build failed")


idf_monitor.Monitor.run_make = alternative_run_make

if __name__ == "__main__":
    idf_monitor.main()
