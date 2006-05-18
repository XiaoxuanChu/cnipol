#! /bin/bash
#rundb_insterter.sh
#April 20, 2006, A. Hoffman

INF=out.dat;
DB=testdb.txt;
LIST=runlist.txt;
test=0;

#RefN=7737.001;

#Find the run number, the next run number, and the reference line number

NLINE=`wc $DB | gawk '{print $1}'`
#grep -n '^.' $LIST > tmplist.txt

grep "\[" $DB | sed -e 's/\[//' | sed -e 's/\]//' > tmplist.txt
DbN=`wc tmplist.txt | gawk '{ print $1 }'`
echo -e "DbN is $DbN"

RunN=`grep "^\[" $INF | sed -e "s/\[//" | sed "s/\]//" | sed -e "s/@//"`
echo -e "Run number is $RunN"


#############################################################################
#                                InsertTag                                  #
#############################################################################


InsertTag()  {

    header=`grep -n "^\[$RefN" testdb.txt | sed -e "s/:/ /" | gawk '{print $1-1}'`
    echo -e "$header";
    head -n $header $DB > tmp1.txt;

    grep '^.' $INF >> tmp1.txt;
    echo -e "" >> tmp1.txt;
    residual=$(( $NLINE - $header ));
    echo -e "$residual";
    tail -n $residual $DB >> tmp1.txt;

    echo "insertion complete"
}

#############################################################################
#                                  AppendTag                                #
#############################################################################

AppendTag() {

    head -n $NLINE $DB > tmp1.txt
    echo "" >> tmp1.txt
    grep '^.' $INF >> tmp1.txt
    echo "appending complete"

}



#############################################################################
#                                    Main                                   #
#############################################################################


for (( i =1; i<=$DbN; i++ )) {
    RefN=`line.sh "$i" tmplist.txt`;
    echo -e "RefN = $RefN";
    test=`CompareRuns $RefN $RunN`
    echo -e "$test";
    if [[ $test == 1 ]]; then
 	break
    fi
} 


if [[ $test == 0 ]]; then
    echo "inserting";
    InsertTag;
    echo "$header";

else
    echo "appending";
    AppendTag;

fi

# mv tmp1.txt $DB
# do not implement the above line until code is perfect.
