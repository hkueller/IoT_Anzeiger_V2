#!/bin/bash

Leistung=$1

TMPFILE=$(mktemp)
cat >$TMPFILE <<EOF
#!/bin/bash

while read -n 1 PROMPT; do
	if [ \$PROMPT == "\$" ]; then
		echo "SetLeistung $Leistung"
		read ANSWER
		echo >&2 $ANSWER
		echo "exit"
		exit
	fi
done
EOF
chmod 755 $TMPFILE
nc display 5023 -c $TMPFILE
rm $TMPFILE
