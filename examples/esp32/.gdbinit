set $esp_reset = 0
define hook-stop
    if ($esp_reset == 0)
	set $esp_reset = 1
	printf "Reset ESP with halt command\n"
	stop
	mon reset halt
	flushregs
	thb app_main
    end
end
