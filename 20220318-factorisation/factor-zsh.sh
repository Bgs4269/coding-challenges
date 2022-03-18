#!/bin/zsh

i=${1}
i=$((${i}*1.0))

typeset -A factors
 
# Find 'em
/usr/games/primes 1 ${$((${i}**.5))%.*} | while read factor; do
  while true; do 
    div=$((${i}/${factor}))
    [[ $div[-1] == "." ]] &&
      {
        i=${div}
        (( factors[${factor}]+=1 ))
      } || {
        break
      }
    [[ ${div} == "1." ]] && break
  done
  [[ ${i} == "1." ]] && break
done

[[ $div != 1 ]] && (( factors[${i}]+=1 ))

# Print 'em
echo ${(k)factors}| tr ' ' '\n' | sort -n | egrep -v "^1.$"| while read f; do
  factorString=${factorString}$(printf "%s^%s " $f $factors[$f])
done
echo ${factorString% }
