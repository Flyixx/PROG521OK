# !/bin/bash

./mainReloc $1 $2 .text 32 .data 10240

./mainPrint -S $1 >test_logs/ex_S.txt
./mainPrint -S $2 >test_logs/z_S.txt

diff test_logs/ex_S.txt test_logs/z_S.txt >test_logs/Slog.txt
if [ -s test_logs/Slog.txt ]; then
    echo "C'est bon pour SectionTab"
else
    echo "ATTENTION: Pas de différence pour SectionTab"
fi

./mainPrint -s $1 >test_logs/ex_s.txt
./mainPrint -s $2 >test_logs/z_s.txt

diff test_logs/ex_s.txt test_logs/z_s.txt >test_logs/slog.txt
if [ -s test_logs/slog.txt ]; then
    echo "C'est bon pour Symtab"
else
     echo "ATTENTION: Pas de différence pour Symtab"
fi

./mainPrint -x $3 $1 >test_logs/ex_x.txt
./mainPrint -x $3 $2 >test_logs/z_x.txt

diff test_logs/ex_x.txt test_logs/z_x.txt >test_logs/xlog.txt
if [ -s test_logs/xlog.txt ]; then
    echo "C'est bon pour contenu hexa"
else
    echo "ATTENTION: Pas de différence pour contenu hexa"
fi