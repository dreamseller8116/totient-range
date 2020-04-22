if [[ "$#" -lt 1 ]]; then
    echo "Error: needs 1 argument (filename)." 1>&2
    exit 128
fi

firstFile=1

[[ -f "$1" ]] && rm $1

for input in $(find . -type f -name '*d-v*.csv')
do
    firstLine=1

    while IFS= read -r line; do
        if [[ firstFile -eq 1 ]] || [[ firstLine -ne 1 ]]; then
            echo "$line" >> $1
        fi

        firstLine=0
    done < "$input"

    firstFile=0
done
