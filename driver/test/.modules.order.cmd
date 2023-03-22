cmd_/media/sf_cmpt433/driver/test/modules.order := {   echo /media/sf_cmpt433/driver/test/testdriver.ko; :; } | awk '!x[$$0]++' - > /media/sf_cmpt433/driver/test/modules.order
