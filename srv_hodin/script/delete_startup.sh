#!/bin/bash

echo -e ''

cd /usr/bin/
rm -rf srv_hodin
echo -e "srv_hodin have been deleted from /usr/bin/"

cd /usr/sbin/
rm -rf srv_hodin
echo -e "srv_hodin have been deleted from /usr/sbin/"

cd /bin/
rm -rf srv_hodin
echo -e "srv_hodin have been deleted from /bin/"

cd /etc/init.d/
rm -rf hodin_daemon.sh
echo -e "hodin_daemon.sh have been deleted from /etc/init.d/"

cd /etc/rc2.d/

rm -rf 'S02hodin_daemon.sh'
echo -e "S02hodin_daemon.sh have been deleted from /etc/rc2.d/"

rm -rf 'S03hodin_daemon.sh'
echo -e "S03hodin_daemon.sh have been deleted from /etc/rc2.d/"

rm -rf 'S88hodin_daemon.sh'
echo -e "S88hodin_daemon.sh have been deleted from /etc/rc2.d/"

cd /etc/rc3.d/

rm -rf 'S02hodin_daemon.sh'
echo -e "S02hodin_daemon.sh have been deleted from /etc/rc3.d/"

rm -rf 'S03hodin_daemon.sh'
echo -e "S03hodin_daemon.sh have been deleted from /etc/rc3.d/"

cd /etc/rc4.d/

rm -rf 'S02hodin_daemon.sh'
echo -e "S02hodin_daemon.sh have been deleted from/ etc/rc4.d/"

rm -rf 'S03hodin_daemon.sh'
echo -e "S03hodin_daemon.sh have been deleted from /etc/rc4.d/"


cd /etc/rc5.d/

rm -rf 'S02hodin_daemon.sh'
echo -e "S02hodin_daemon.sh have been deleted from /etc/rc5.d/"

rm -rf 'S03hodin_daemon.sh'
echo -e "S03hodin_daemon.sh have been deleted from /etc/rc5.d/"

echo -e ''

echo -e "srv_hodin have been completly removed from startup routine."
echo -e ''
