# dyndns-update
Keep dynamic DNS server up to date

This program is written to keep a www.dynu.com dynamic DNS record up to date. Tested on macos and Raspian for the RaspberryPi.

This should be run via crontab or similar, a sample crontab entry (every 15 minutes) might look like

```
# Example of job definition:
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * user-name command to be executed
*/15  * * * *   root    /usr/local/bin/ddnsupd.sh /home/user1 >> /home/user1/.ddnsupd/ddnsupd.log
#

```
