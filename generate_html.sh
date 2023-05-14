src="settings.html"

if [ ! -f $src ]; then echo "src error"; exit; fi

name=$(echo $src | sed "s|\.|_|g")
dst=${name}.h

if [ -f $dst ]; then rm $dst; fi

echo "#ifndef ${name^^}" >> $dst
echo "#define ${name^^}" >> $dst
echo "" 		         >> $dst
echo "const char $name[] PROGMEM = R\"rawliteral(" >> $dst

cat $src | sed -e 's/^[ \t]*//' | awk '{ if ($0 != "") print $0 }' >> $dst

echo ")rawliteral\";"    >> $dst
echo "" 		         >> $dst
echo "#endif" 		     >> $dst

sed -i "s|%|%%|g" $dst
sed -i "s|%%STA_SSID%%|%STA_SSID%|g" $dst
sed -i "s|%%STA_PASS%%|%STA_PASS%|g" $dst
sed -i "s|%%STA_HOST%%|%STA_HOST%|g" $dst
sed -i "s|%%STA_PORT%%|%STA_PORT%|g" $dst
sed -i "s|%%STA_WIFI_CONN%%|%STA_WIFI_CONN%|g" $dst
sed -i "s|%%STA_API_CONN%%|%STA_API_CONN%|g" $dst
sed -i "s|%%STA_WIFI_CONN_CLASS%%|%STA_WIFI_CONN_CLASS%|g" $dst
sed -i "s|%%STA_API_CONN_CLASS%%|%STA_API_CONN_CLASS%|g" $dst


