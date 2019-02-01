set -e 
set -o pipefail

echo "Reading samples from 'localSamples.conf'"


while read line;
do

    if [ -z "$line" ]; then
        continue
    else 
        [[ "$line" =~ ^'#'.*$ ]] && continue
        echo $line
        root -l -b -q 'RunAnalyserPAF.C("'${line}'"      ,"WZ",  20, -5)'

    fi

done < localSamples.conf



