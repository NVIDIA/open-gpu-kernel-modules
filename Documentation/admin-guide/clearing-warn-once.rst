Clearing WARN_ONCE
------------------

WARN_ONCE / WARN_ON_ONCE / printk_once only emit a message once.

echo 1 > /sys/kernel/debug/clear_warn_once

clears the state and allows the warnings to print once again.
This can be useful after test suite runs to reproduce problems.
