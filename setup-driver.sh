# this file was generated by calling ./data2setup.sh 
# DO NOT EDIT

    dialog  --clear --backtitle "$BACKTITLE" \
            --title "Select your driver" \
            --menu "$CONFIG_DRIVER_TEXT" 18 74 11 \
               \
			1 "Home-brew (16x50 UART compatible serial port)" \
			2 "Home-brew (parallel port)" \
			3 "Home-brew (soundcard input)" \
			4 "Other serial port devices" \
			5 "TV card" \
			6 "IrDA hardware" \
			7 "PDAs" \
			8 "USB devices" \
			9 "Network (UDP)" \
			0 "Other (MIDI, Bluetooth, etc.)" \
			a "None (network connections only)" 2> $TEMP
    if test "$?" = "0"; then
        {
        set `cat $TEMP`
        if false; then :
        elif test "$1" = "1"; then LIRC_DRIVER=serial;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
        elif test "$1" = "2"; then LIRC_DRIVER=parallel;	DRIVER_PARAMETER=lpt1; DRIVER_PARAM_TYPE=lpt;
        elif test "$1" = "3"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 10 74 3 \
                       \
			1 "Simple IR diode (EXPERIMENTAL)" \
			2 "IR receiver IC connected to audio input (EXPERIMENTAL)" \
			3 "IR receiver IC connected to audio input using ALSA (EXPERIMENTAL)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=dsp;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=audio;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=audio_alsa;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "4"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 19 74 12 \
                       \
			1 "Anir Multimedia Magic" \
			2 "CARACA" \
			3 "Creative Infra Receiver/CIMR100" \
			4 "IRdeo" \
			5 "IRdeo Remote" \
			6 "Irman / UIR" \
			7 "Kanam Accent" \
			8 "Linksys NSLU2 (CTS2+GreenLED)" \
			9 "Logitech/AST" \
			0 "PCMAK serial receiver" \
			a "Miro PCTV receiver" \
			b "Packard Bell receiver" \
			c "Pinnacle Systems PCTV (pro) receiver" \
			d "Pinnacle Systems PCTV Sat receiver" \
			e "PixelView RemoteMaster RC2000/RC3000" \
			f "REALmagic (bundled with Hollywood Plus DVD card)" \
			g "Slink-e (receive only)" \
			h "Silitek SM-1000" \
			i "Tekram Irmate 210 (16x50 UART compatible serial port)" \
			j "UIRT2 (receive only, UIR mode)" \
			k "UIRT2 (receive and transmit)" \
			l "X10 MouseRemote RF Receiver (Serial)" \
			m "X10 MouseRemote RF Receiver (PS/2)" \
			n "X10 MP3 Anywhere RF Receiver" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=animax;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "2"; then LIRC_DRIVER=caraca;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "3"; then LIRC_DRIVER=creative;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "4"; then LIRC_DRIVER=irdeo;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "5"; then LIRC_DRIVER=irdeo_remote;	DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "6"; then LIRC_DRIVER=irman;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "7"; then LIRC_DRIVER=accent;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "8"; then LIRC_DRIVER=nslu2;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "9"; then LIRC_DRIVER=logitech;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "0"; then LIRC_DRIVER=pcmak;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "a"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "b"; then LIRC_DRIVER=packard_bell;	DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "c"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "d"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "e"; then LIRC_DRIVER=remotemaster;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "f"; then LIRC_DRIVER=realmagic;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "g"; then LIRC_DRIVER=slinke;		DRIVER_PARAMETER=tty3; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "h"; then LIRC_DRIVER=silitek;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "i"; then LIRC_DRIVER=tekram;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "j"; then LIRC_DRIVER=uirt2;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "k"; then LIRC_DRIVER=uirt2_raw;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "l"; then LIRC_DRIVER=mouseremote;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "m"; then LIRC_DRIVER=mouseremote_ps2;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "n"; then LIRC_DRIVER=mp3anywhere;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "5"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 19 74 12 \
                       \
			1 "Adaptec AVC-2410" \
			2 "Askey Magic TView CPH03x (card=1)" \
			3 "Askey/Typhoon/Anubis Magic TView CPH051/061 (bt878) (card=24)" \
			4 "Asus TV-Box" \
			5 "AverMedia TV card (TVCapture, TVPhone) (card=6)" \
			6 "AverMedia TV card (TVCapture98, TVPhone98) (card=13/41)" \
			7 "AverMedia TV card (VDOMATE) (use card=13)" \
			8 "BestBuy Easy TV (BT848) (card=55)" \
			9 "BestBuy Easy TV (BT878) (card=62)" \
			0 "Chronos Video Shuttle II (card=35)" \
			a "Creative BreakOut-Box" \
			b "Dynalink Magic TView (card=48)" \
			c "FlyVideo II (card=8)" \
			d "FlyVideo 98 (card=30)" \
			e "FlyVideo 98/FM /2000S (card=56)" \
			f "Flyvideo 98FM (LR50Q) / Typhoon TView TV/FM Tuner (card=36)" \
			g "Hauppauge TV card" \
			h "Hauppauge DVB-s card (ver. 2.1)" \
			i "Hercules Smart TV Stereo (card=100)" \
			j "I-O Data Co. GV-BCTV5/PCI (card=81)" \
			k "Jetway TV/Capture JW-TV878-FBK, Kworld KW-TV878RF (card=78)" \
			l "KNC ONE TV Station (-/SE/PRO/RDS)" \
			m "Lenco MXTV-9578 CP (card=50)" \
			n "Miro PCTV serial port receiver" \
			o "Phoebe Tv Master + FM (card=22)" \
			p "Pinnacle Systems PCTV Sat receiver" \
			q "Pixelview PlayTV MPEG2" \
			r "PixelView PlayTV PAK (card=50)" \
			s "Pixelview PlayTV pro (card=37)" \
			t "Pixelview PlayTV (bt878) (Prolink PV-BT878P+, card=16)" \
			u "Prolink Pixelview PV-BT878P+ (Rev.4C,8E, card=70)" \
			v "Prolink PV-BT878P+4E (card=50)" \
			w "Prolink PV-BT878P+9B (PlayTV Pro rev.9B FM+NICAM) (card=72)" \
			x "ProVideo PV951 (card=42)" \
			y "Technisat MediaFocus I" \
			z "Tekram M230 Mach64 (and others bt829 based)" \
			A "TriTan Technology TView95 CPH03x (card=1)" \
			B "TView99 CPH063 (card=38)" \
			C "Typhoon TView RDS / FM Stereo (card=53)" \
			D "Winfast PVR2000 (Linux kernel >=2.6.11 required)" \
			E "Winfast TV2000/XP (card=34)" \
			F "WinView 601 (card=17)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=adaptec;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=tvbox;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=avermedia;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "6"; then LIRC_DRIVER=avermedia98;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "7"; then LIRC_DRIVER=avermedia_vdomate;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "8"; then LIRC_DRIVER=bestbuy;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "9"; then LIRC_DRIVER=bestbuy2;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "0"; then LIRC_DRIVER=chronos;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "a"; then LIRC_DRIVER=breakoutbox;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "b"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "c"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "d"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "e"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "f"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "g"; then LIRC_DRIVER=hauppauge;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "h"; then LIRC_DRIVER=hauppauge_dvb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "i"; then LIRC_DRIVER=hercules_smarttv_stereo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "j"; then LIRC_DRIVER=gvbctv5pci;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "k"; then LIRC_DRIVER=kworld;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "l"; then LIRC_DRIVER=knc_one;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "m"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "n"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "o"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "p"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "q"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "r"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "s"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "t"; then LIRC_DRIVER=pixelview_bt878;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "u"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "v"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "w"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "x"; then LIRC_DRIVER=provideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "y"; then LIRC_DRIVER=mediafocusI;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "z"; then LIRC_DRIVER=tekram_bt829;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "A"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "B"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "C"; then LIRC_DRIVER=knc_one;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "D"; then LIRC_DRIVER=leadtek_pvr2000;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "E"; then LIRC_DRIVER=leadtek_0010;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "F"; then LIRC_DRIVER=leadtek_0007;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "6"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 13 74 6 \
                       \
			1 "SIR IrDA (built-in IR ports)" \
			2 "Tekram Irmate 210 (16x50 UART compatible serial port)" \
			3 "ITE IT8712/IT8705 CIR port" \
			4 "Asus Digimatrix IT87xx CIR port" \
			5 "Actisys Act200L SIR driver support" \
			6 "Actisys Act220L(+) SIR driver support" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=sir;		DRIVER_PARAMETER=com3; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "2"; then LIRC_DRIVER=tekram;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "3"; then LIRC_DRIVER=it87;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=digimatrix;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=act200l;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "6"; then LIRC_DRIVER=act220l;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "7"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 9 74 2 \
                       \
			1 "HP iPAQ" \
			2 "Sharp Zaurus" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=sa1100;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=sa1100;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "8"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 19 74 12 \
                       \
			1 "ADSTech USBX-707 USB IR Blaster" \
			2 "Apple Mac mini USB IR Receiver" \
			3 "Asus DH USB Remote" \
			4 "ATI/NVidia/X10 I & II RF Remote" \
			5 "ATI/NVidia/X10 RF Remote (userspace)" \
			6 "Creative USB IR Receiver (SB0540)" \
			7 "COMMANDIR USB Transceiver" \
			8 "Dign HV5 HTPC IR/VFD Module" \
			9 "DViCO USB Remote" \
			0 "Home Electronics Tira USB device" \
			a "Igor Cesko's USB IR Receiver" \
			b "Iguanaworks USB IR Transceiver" \
			c "PCMAK USB receiver" \
			d "Remotec Multimedia PC Remote BW6130" \
			e "Sasem OnAir Remocon-V" \
			f "Sound Blaster Extigy USB sound card (exaudio)" \
			g "Sound Blaster Extigy/Audigy 2 NX (ALSA snd-usb-audio)" \
			h "Soundgraph iMON 2.4G DT & LT" \
			i "Soundgraph iMON MultiMedian IR/VFD" \
			j "Soundgraph iMON Knob" \
			k "Soundgraph iMON PAD IR/VFD" \
			l "Soundgraph iMON RSC" \
			m "Streamzap PC Remote" \
			n "TechnoTrend USB IR receiver" \
			o "USB-UIRT" \
			p "Windows Media Center Remotes (old version, MicroSoft USB ID)" \
			q "Windows Media Center Remotes (new version, Philips et al.)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=usbx;		DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "2"; then LIRC_DRIVER=macmini;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=asusdh;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=atiusb;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=atilibusb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "6"; then LIRC_DRIVER=sb0540;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "7"; then LIRC_DRIVER=cmdir;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "8"; then LIRC_DRIVER=sasem;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "9"; then LIRC_DRIVER=dvico;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "0"; then LIRC_DRIVER=tira;		DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "a"; then LIRC_DRIVER=igorplugusb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "b"; then LIRC_DRIVER=iguanaIR;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "c"; then LIRC_DRIVER=pcmak_usb;	DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "d"; then LIRC_DRIVER=bw6130;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "e"; then LIRC_DRIVER=sasem;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "f"; then LIRC_DRIVER=exaudio;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "g"; then LIRC_DRIVER=alsa_usb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "h"; then LIRC_DRIVER=imon_24g;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "i"; then LIRC_DRIVER=imon;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "j"; then LIRC_DRIVER=imon_knob;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "k"; then LIRC_DRIVER=imon_pad;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "l"; then LIRC_DRIVER=imon_rsc;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "m"; then LIRC_DRIVER=streamzap;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "n"; then LIRC_DRIVER=ttusbir;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "o"; then LIRC_DRIVER=usb_uirt_raw;	DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "p"; then LIRC_DRIVER=mceusb;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "q"; then LIRC_DRIVER=mceusb2;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "9"; then LIRC_DRIVER=udp;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
        elif test "$1" = "0"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 13 74 6 \
                       \
			1 "AOpen XC Cube EA65, EA65-II" \
			2 "Creative LiveDrive midi" \
			3 "Creative LiveDrive sequencer" \
			4 "Creative iNFRA CDROM" \
			5 "Ericsson mobile phone via Bluetooth" \
			6 "Linux input layer (/dev/input/eventX)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=ea65;		DRIVER_PARAMETER=tty2; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "2"; then LIRC_DRIVER=livedrive_midi;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=livedrive_seq;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=creative_infracd;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=bte;		DRIVER_PARAMETER=btty; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "6"; then LIRC_DRIVER=devinput;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "a"; then LIRC_DRIVER=none;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
        fi
        }
        else
            return;
        fi;
