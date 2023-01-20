sudo iptables --table nat --append POSTROUTING --out-interface enp0s3 -j MASQUERADE
sudo iptables --append FORWARD --in-interface enx8030dc9359be -j ACCEPT
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

ssh debian@192.168.7.2
sudo route add default gw 192.168.7.1
echo nameserver 8.8.8.8 | sudo tee -a /etc/resolv.conf
ping google.ca
