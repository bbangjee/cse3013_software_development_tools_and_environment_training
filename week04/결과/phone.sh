if [ $# -lt 1 ]
then
  echo "Usage: phone searchfor [...searchfor]"
  echo "(You didn't tell me what you want to search for.)"
  exit 0
else
  grep -i "$1" mydata.txt > tmp

  shift
  for arg in "$@"
  do
    grep -i "$arg" tmp > tmp.new
    mv tmp.new tmp
  done

  awk -f display.awk tmp
fi