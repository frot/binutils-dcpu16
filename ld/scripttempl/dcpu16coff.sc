if [ x${LD_FLAG} = x ]
then
cat << EOF

OUTPUT_FORMAT("binary")
. = 0x0000;
__Ltext = .;
ENTRY (__Ltext)
EOF
else 
    echo "OUTPUT_FORMAT(\"${OUTPUT_FORMAT}\")"
fi
cat <<EOF
OUTPUT_ARCH("${OUTPUT_ARCH}")
SECTIONS
{
.text :	{
	*(.text)
	*(text)
	${RELOCATING+ __Htext = .;}
	}
.data :	{
	${RELOCATING+ __Ldata = .;}
	*(.data)
	*(data)
	${RELOCATING+ __Hdata = .;}
	}
.bss :	{
	${RELOCATING+ __Lbss = .;}
	*(.bss)
	*(bss)
	${RELOCATING+ *(COMMON)}
	${RELOCATING+ __Hbss = .;}
	}
	${RELOCATING+ __Lheap = .;}
}
EOF
