if [ $# -lt 1 ]
then
  echo "Usage: phone searchfor [...searchfor]"
  echo "(You didn't tell me what you want to search for.)"
  exit 0
else
  args=$1
  for arg in "$@"
  do
    args="${args}|${arg}"
  done
  
  egrep -i "$args" mydata.txt > tmp
  awk -f display.awk tmp
fi