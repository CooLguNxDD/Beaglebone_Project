sudo iptables --table nat --append POSTROUTING --out-interface enp0s3 -j MASQUERADE

sudo iptables --append FORWARD --in-interface enx8030dc9359be -j ACCEPT

sudo echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward