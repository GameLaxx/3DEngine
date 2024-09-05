cd "$(dirname "$0")"

if [ -z $1 ]; then
    echo "You should provide a file name."
    exit 1
fi
LOWER=$1
UPPER=${LOWER^^}
cp template.h ../src/$1.h
cp template.c ../src/$1.c
sed -i -e 's/$$TEMPLATE$$_H/'$UPPER'_H/g' ../src/$1.h
sed -i -e 's/$$template$$.h/'$LOWER'.h/g' ../src/$1.h
sed -i -e 's/\"$$template$$.h\"/\"'$LOWER'.h\"/g' ../src/$1.c
echo "Your file "$1".c and "$1".h are ready to use !"