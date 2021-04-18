PSP Mac Spoofer.

This is a plugin to temporally change the mac address that your psp sends to Access Points or other wifi devices.

This plugin does NOT:

- Change the mac address stored in idstorage (that's useless, as that is just to show the inf in xmb). Therefore you will still see your old mac in system info.
- Change the read only mac address stored in the wlan chip.

This plugin DOES:

- Change the mac address that your psp will send to other wireless devices.
  This is useful if you want to hide your real mac address to other people for some reason,
  or if you want to bypass some mac filter protection of your neighboor :)

  You can see your spoofed mac address when you test a connection in the XMB, or if you look into router logs.
  
  By default, the Sony firmware "tells" the wlan chip firmware to use the internal mac address.
  This plugin just changes that.

Requeriments:

- 3.71 M33. It may work in future M33 firmwares but it is not guaranteed.

Instructions:

- Copy PSP directory to your memory stick. Execute the MAC Spoofer install program.
  This program will reflash the configuration files changing a bit the modules load order. 
  This is needed because the macspoofer plugin needs to be executed before wlan.prx.

- Copy seplugins folder yo tour memory stick, and activate the plugin in recovery in both, vsh and game.


The mac address will be spoofed to the address contained in /seplugins/mac.txt
You can use the characters RR to specify that macspoofer should randomize that byte.

- Examples of valid macs:

33:33:33:33:33:33   -> It sets that specific mac, a mac certified by M33 Team :p

00:1c:26:RR:RR:RR   -> It sets the first 3 bytes of the mac to 00:1c:26, which is the code of 
Hon Hai/Foxconn, the manufacturer of the psp slim chip, and randomizes the last 3 bytes.

00:16:FE:RR:RR:RR   -> It sets the first 3 bytes of the mac to 00:16:FE, which is the code of Alps Electric,
the manufacturer of (some) fat psp chip, and randomizes the last 3 bytes.

00:RR:RR:RR:RR:RR -> First byte is set to 00 to look like a good mac, and the rest is randomized.


If you want to use the code of X manufacturer, just take a look at http://standards.ieee.org/regauth/oui/oui.txt

- Example of bad macs:

- 333333333333 -> Separator ":" is missing.
- 00:2E:4R:FF:22:DE -> The random cannot be applied to only the half of a byte.
- 00:2G:RR:FF:DA:E1 -> "G" is not a valid hexadecimal char.
- 00:1C:26:33:12 -> a byte of the mac is missing

To go back to your old mac, you can set the mac FF:FF:FF:FF:FF:FF or just deactivate the plugin.

Note: the plugin will be uninstalled when reflashing a full cfw. You will have to do the process
again in a new firmware, although by the patching nature of the program, it may not work in future firmwares without an update.




