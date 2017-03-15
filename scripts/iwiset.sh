if [ $# -ne 1 ]; then
    echo "Usage: iwiset [up|down|conn]"
else
    if [ $* = "up" ]; then
	kldload if_iwi
	iwicontrol -i iwi0 -d /boot/firmware/ -m bss
	ifconfig iwi0 up
    elif [ $* = "down" ]; then 
	iwicontrol -i iwi0 -k
	ifconfig iwi0 down
	kldunload if_iwi
    elif [ $* = "conn" ]; then
	ifconfig iwi0 ssid GIT
	dhclient -q iwi0
    else
	echo "Usage: iwiset [up|down|conn]"
    fi
fi
