output="totient.csv"
firstFile=1

[[ -f $output ]] && rm $output

for input in $(find . -type f -name '*.csv'); do
    firstLine=1

    while IFS= read -r line; do
        if [[ firstFile -eq 1 ]] || [[ firstLine -ne 1 ]]; then
            echo "$line" >> $output
        fi

        firstLine=0
    done < "$input"

    firstFile=0
done
