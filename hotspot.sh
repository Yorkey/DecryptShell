#!/bin/sh
which wifidog 2>&1 >/dev/null || exit 1
DOCUMENT_ROOT=$(main.sbin www_root)
check_offline()
{
ping -c3 www.baidu.com | grep -q ttl
if
[ $? -ne 0 ]
then
grep -q "^address=/#/114.114.114.114" /etc/dnsmasq.conf
if
[ $? -ne 0 ]
then
echo "address=/#/114.114.114.114" >> /etc/dnsmasq.conf
/etc/init.d/dnsmasq restart 2>&1 >/dev/null &
ip route | grep -q default
#	if
#	[ $? -ne 0 ]
#	then
#	killall wifidog
#	ifconfig eth0.2 172.172.172.254
#	route add -net default gw 172.172.172.1
#	killall wifidog
#	/etc/init.d/wifidog start
#	sleep 3
#	ifconfig eth0.2 0.0.0.0
#	fi
fi
iptables-save -t nat | grep -q "hotspot enable_offline" || iptables -t nat -I PREROUTING -i br-lan -p tcp -m tcp --dport 80 ! -d $(uci get network.lan.ipaddr) -j  REDIRECT --to-ports 65010 -m comment --comment "hotspot enable_offline"
else
if
grep -q "^address=/#/114.114.114.114" /etc/dnsmasq.conf
then
sed -i '/^address=/d' /etc/dnsmasq.conf
/etc/init.d/dnsmasq restart 2>&1 >/dev/null &
fi
iptables-save -t nat | grep -q "hotspot enable_offline" && iptables -t nat -D PREROUTING -i br-lan -p tcp -m tcp --dport 80 ! -d $(uci get network.lan.ipaddr) -j  REDIRECT --to-ports 65010 -m comment --comment "hotspot enable_offline"
fi
}
# interrupt_auth()
# {
# date_min_now=$(expr $(date +%s) / 60)
# ls /tmp/wifidog_users/ | grep -Eo "^[0-9]*" | while read date_min
# do
# if
# [ $date_min -lt $date_min_now ]
# then
# cat /tmp/wifidog_users/${date_min}.users | while read ip mac
	# do
	# iptables -t mangle -D WiFiDog_br-lan_Incoming -d ${ip}/32 -j ACCEPT	2>&1 >/dev/null
	# iptables -t mangle -D WiFiDog_br-lan_Outgoing -s ${ip}/32 -m mac --mac-source ${mac} -j MARK --set-xmark 0x2/0xffffffff 2>&1 >/dev/null
	# done
# rm -f /tmp/wifidog_users/${date_min}.users
# fi
# done
# }
# auth()
# {
# auth 192.168.1.213 08:00:27:a0:18:1b 2

# #iptables -t mangle -D WiFiDog_br-lan_Incoming -d 192.168.1.213/32 -j ACCEPT
# #iptables -t mangle -D WiFiDog_br-lan_Outgoing -s 192.168.1.213/32 -m mac --mac-source 08:00:27:A0:18:1B -j MARK --set-xmark 0x2/0xffffffff
# eval `cat $DOCUMENT_ROOT/apps/hotspot/hotspot.conf`
# expit_data=$(expr $(expr $(date +%s) / 60) + $force_reauth)
# [ -d /tmp/wifidog_users ] || mkdir /tmp/wifidog_users
# echo  "$2 $3" >> /tmp/wifidog_users/$expit_data.users
# }
allow_ips()
{
old_fw_trust=`iptables-save -t mangle | grep "WiFiDog_br-lan_Trusted .* --set-xmark 0x2/0xffffffff$" | grep -Eo '..:..:..:..:..:..|[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'`
[ -z "$DOCUMENT_ROOT" ] && DOCUMENT_ROOT=$(main.sbin www_root)
grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' $DOCUMENT_ROOT/apps/hotspot/white_local_ips.conf | while read trust_ip
do
echo "$old_fw_trust" | grep -q "^${trust_ip}$" || iptables -t mangle -I WiFiDog_br-lan_Trusted 1 -s ${trust_ip} -j MARK --set-xmark 0x2/0xffffffff
done
}
do_white_mac_set()
{
iptables -t mangle -F WiFiDog_br-lan_Trusted
# old_fw_trust=`iptables-save -t mangle | grep "WiFiDog_br-lan_Trusted .* --set-xmark 0x2/0xffffffff$" | grep -Eo '..:..:..:..:..:..|[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'`
sed -i '/^TrustedMACList /d' /etc/wifidog.conf
echo "TrustedMACList $(echo "$FORM_trust_mac_list" | grep -Eo '..:..:..:..:..:..' | tr '\n' ',' | sed 's/,$//')" >> /etc/wifidog.conf
echo "$FORM_trust_mac_list" | grep -Eo '..:..:..:..:..:..' | while read trust_mac
do
# echo "$old_fw_trust" | grep -q "^${trust_mac}$" || 
iptables -t mangle -I WiFiDog_br-lan_Trusted 1 -m mac --mac-source ${trust_mac} -j MARK --set-xmark 0x2/0xffffffff
done
echo "$FORM_trust_mac_list" | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | while read trust_ip
do
# echo "$old_fw_trust" | grep -q "^${trust_ip}$" || 
iptables -t mangle -I WiFiDog_br-lan_Trusted 1 -s ${trust_ip} -j MARK --set-xmark 0x2/0xffffffff
done
echo "$FORM_trust_mac_list" | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' > $DOCUMENT_ROOT/apps/hotspot/white_local_ips.conf
}
do_berbonsetting()
{
br_lan_mac=`ubus call network.device status '{ "name": "br-lan" }' | grep -Eo '(([0-9 A-Z a-z]{1,2}:){5}[0-9 A-Z a-z]{1,2})'`
slat_string="$br_lan_mac"
token_time=`date +%s`
token_livetime=180
token=`echo -n "$(awk 'BEGIN{result='$token_time'/'$token_livetime';printf "%f\n",result}' | sed 's/\..*//')$slat_string" | md5sum | awk {'print $1'}`

# if
# [ "$token" = "$FORM_token" ] || [ "$token" = "$FORM_token1" ]
# then
sed -i "s/^berbon_id=.*/berbon_id=\"$FORM_hotspot_berbon_id\"/" $DOCUMENT_ROOT/apps/hotspot/hotspot.conf
br_lan_mac_str=`echo "$br_lan_mac" | tr -d ":"`
sed -i "s/^GatewayID .*/GatewayID $br_lan_mac_str/" /etc/wifidog.conf
sed -i "s/berbonid=[a-zA-Z0-9]*\&/berbonid=${FORM_hotspot_berbon_id}\&/g" /etc/wifidog.conf
sed -i "/^gw_id/d" $DOCUMENT_ROOT/apps/hotspot/hotspot.conf
echo "gw_id=\"$br_lan_mac_str\"" >>$DOCUMENT_ROOT/apps/hotspot/hotspot.conf
# fi
}
berbonsetting()
{
br_lan_mac=`ubus call network.device status '{ "name": "br-lan" }' | grep -Eo '(([0-9 A-Z a-z]{1,2}:){5}[0-9 A-Z a-z]{1,2})'`
random_num=`tr -cd 0-9 </dev/urandom | head -c 8`
eval `cat $DOCUMENT_ROOT/apps/hotspot/hotspot.conf`
cat <<EOF

					<label>
					倍棒ID:
						<input type="text" value="$berbon_id" disabled>
						<a type="submit" class="pure-button button-warning" target="_blank" href="${berbon_url_prefix}?gwaddr=$HTTP_HOST:$SERVER_PORT&gwmac=${br_lan_mac}&jiffies=${random_num}">重新登录</a>
					</label>
EOF
}

del_white_domain_list()
{
iptables-save -t filter | grep -E "hotspot white_domain" | while read rule
do
echo "${rule}" | sed 's/-A //' | xargs iptables -t filter -D 
done
ipset -X hotspot_white_domain
iptables -t filter -F WiFiDog_br-lan_Global
}
white_domain_list_flush()
{
ipset flush hotspot_white_domain
}
white_domain_list()
{
while true
do
iptables-save -t filter | grep -F "WiFiDog_br-lan_Global" && break || sleep 1
done
for ip in  `grep -vhE "^$|^#" $(main.sbin www_root)/apps/hotspot/*white_domain_list.conf | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'`
do
iptables -t filter -I WiFiDog_br-lan_Global 1 -d ${ip}/32 -j ACCEPT
iptables -t nat -I WiFiDog_br-lan_Global 1 -d ${ip}/32 -j ACCEPT
done

rm -f /etc/dnsmasq.d/hotspot_white_domain.conf
for domain in `grep -vhE "^$|^#" $(main.sbin www_root)/apps/hotspot/*white_domain_list.conf | cut -d '/' -f2 | grep -Eo '([a-z0-9]|\.).*[a-z]'`
do
cat <<EOF >> /etc/dnsmasq.d/hotspot_white_domain.conf
ipset=/${domain}/hotspot_white_domain
EOF
done
ipset -N hotspot_white_domain iphash >/dev/null 2>&1

iptables -t filter -I WiFiDog_br-lan_Global 1 -m set --match-set hotspot_white_domain dst -j ACCEPT -m comment --comment "hotspot white_domain"
iptables -t nat -I WiFiDog_br-lan_Global 1 -m set --match-set hotspot_white_domain dst -j ACCEPT -m comment --comment "hotspot white_domain"

iptables -t nat -I WiFiDog_br-lan_Global -p tcp --dport 80 -m string --string "hrost" --algo kmp -j ACCEPT -m comment --comment "hotspot white_domain"

# iptables -I FORWARD -m string --string "hrost" --algo kmp -j ACCEPT -m comment --comment "hotspot white_domain"
/etc/init.d/dnsmasq restart 2>&1 >/dev/null &
# if
# ping -c3 www.baidu.com | grep -q ttl
# then
	# if
	# grep -q "^address=/#/8.8.8.8" /etc/dnsmasq.conf
	# then
	# sed -i '/^address=/d' /etc/dnsmasq.conf
	# /etc/init.d/dnsmasq restart
	# iptables -t nat -D WiFiDog_br-lan_WIFI2Internet -p tcp -m tcp --dport 80 -j  REDIRECT --to-ports 2060
	# fi
# else
# grep -q "^address=/#/8.8.8.8" /etc/dnsmasq.conf || (echo "address=/#/8.8.8.8" >> /etc/dnsmasq.conf && /etc/init.d/dnsmasq restart)
# iptables-save -t nat | grep -qF "WiFiDog_br-lan_WIFI2Internet -p tcp -m tcp --dport 80 -j REDIRECT --to-ports 2060" || iptables -t nat -I WiFiDog_br-lan_WIFI2Internet -p tcp -m tcp --dport 80 -j  REDIRECT --to-ports 2060
# return 1
# fi
# white_domain_list_str=$(cat <<EOF)
# $(grep -vE "^$|^#" $DOCUMENT_ROOT/apps/hotspot/white_domain_list.conf)
# $(grep -vE "^$|^#" $DOCUMENT_ROOT/apps/hotspot/hotspot_white_domain_list.conf)
# EOF

# white_list_iptables=`iptables-save -t filter | grep -E "WiFiDog_br-lan_Global -d ([0-9]{1,3}[\.]){3}[0-9]{1,3}/32 -j ACCEPT" | grep -Eo "([0-9]{1,3}[\.]){3}[0-9]{1,3}" | sort -n | uniq | grep -v "^$"`
# if
# [ $(echo "$white_list_iptables" | wc -l) -lt 10 ]
# then
# ip_now=`for domain in $white_domain_list_str
# do
# ip=$(ping -q -W 1 -w 1 -c 1 ${domain} | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')
# echo ${ip}
# done`
# else
# ip_now=`for domain in $white_domain_list_str
# do
# ip=$(nslookup ${domain} | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | grep -v '^127.0.0.1$')
# echo "${ip}"
# done`
# fi
# ips_1=`cat /tmp/white_list.1`
# ips_2=`cat /tmp/white_list.2`
# ips_3=`cat /tmp/white_list.3`
# ips_4=`cat /tmp/white_list.4`
# white_list=$(cat <<EOF | sort -n | uniq | grep -v "^$")
# $ip_now
# $ips_1
# $ips_2
# $ips_3
# $ips_4
# EOF

# [ "$(echo "$white_list" | md5sum | cut -d ' ' -f1)" = "$(echo "$white_list_iptables" | md5sum | cut -d ' ' -f1)" ] && exit 0

# new_add_ips=$white_list
# for old_ip in $white_list_iptables
# do
# new_add_ips=`echo "$new_add_ips" | grep -v "^${old_ip}$"`
# done
# old_del_ips=$white_list_iptables
# for new_ip in $white_list
# do
# old_del_ips=`echo "$old_del_ips" | grep -v "^${new_ip}$"`
# done

# [ -n "$old_del_ips" ] && for old_del_ip in $old_del_ips
# do
# iptables -t filter -D WiFiDog_br-lan_Global -d ${old_del_ip}/32 -j ACCEPT
# iptables -t nat -D WiFiDog_br-lan_Global -d ${old_del_ip}/32 -j ACCEPT
# done
# for new_add_ip in $new_add_ips
# do
# iptables -t filter -I WiFiDog_br-lan_Global 1 -d ${new_add_ip}/32 -j ACCEPT
# iptables -t nat -I WiFiDog_br-lan_Global 1 -d ${new_add_ip}/32 -j ACCEPT
# done

# [ ! -f /tmp/white_list.4 ] && echo "$ip_now" > /tmp/white_list.4
# [ ! -f /tmp/white_list.3 ] && echo "$ip_now" > /tmp/white_list.3
# [ ! -f /tmp/white_list.2 ] && echo "$ip_now" > /tmp/white_list.2
# [ ! -f /tmp/white_list.1 ] && echo "$ip_now" > /tmp/white_list.1
# rm -f /tmp/white_list.4
# mv /tmp/white_list.3 /tmp/white_list.4
# mv /tmp/white_list.2 /tmp/white_list.3
# mv /tmp/white_list.1 /tmp/white_list.2
# echo "$ip_now" > /tmp/white_list.1
# iptables-save -t nat | grep "WiFiDog_br-lan_Global -d " | grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | sort -n | uniq -d | while read ip
# do
# iptables -t filter -D WiFiDog_br-lan_Global -d ${ip}/32 -j ACCEPT
# iptables -t nat -D WiFiDog_br-lan_Global -d ${ip}/32 -j ACCEPT
# done
}
trust_mac_list()
{
for mac in `grep "^TrustedMACList " /etc/wifidog.conf | sed -n 1p | awk '{print $2}' | sed 's/,/\n/g'`
do
echo ${mac}
done
}
respone()
{
# for i in `ls /tmp | grep -E "[a-z|A-Z|0-9|-|_]-([a-z0-9]{12})-hotspot_(login|auth|ping).log"`
# do
# rm /tmp/${i}
# done
eval `cat $(main.sbin www_root)/apps/hotspot/hotspot.conf`
mkdir /tmp/${berbon_id}-${gw_id}_hotspot
for file in hotspot_auth.log hotspot_login.log hotspot_ping.log
do
[ -f /tmp/${file} ] && cp /tmp/${file} /tmp/${berbon_id}-${gw_id}_hotspot
done
cd /tmp && tar czf /tmp/${berbon_id}-${gw_id}_hotspot.tar.gz ${berbon_id}-${gw_id}_hotspot && rm -rf /tmp/${berbon_id}-${gw_id}_hotspot
}

case $1 in
do_berbonsetting)
do_berbonsetting
;;
berbonsetting)
berbonsetting
;;
white_domain_list)
white_domain_list
;;
del_white_domain_list)
del_white_domain_list
;;
trust_mac_list)
trust_mac_list
;;
do_white_mac_set)
do_white_mac_set
;;
allow_ips)
allow_ips
;;
white_domain_list_fush)
white_domain_list_fush
;;
check_offline)
check_offline
;;
respone)
respone
;;
# auth)
# auth $@
# ;;
# interrupt_auth)
# interrupt_auth
# ;;
esac
