#!/bin/zsh

#DEBUG=1

file=${1}

function debug()
{
  [[ ! -z ${DEBUG} ]] &&
    {
      echo "DEBUG: $*" 1>&2
      return
    }
}


function usage()
{
  echo "polynome-aggragator <input-file>"
  echo "  Input file must contain one polynome per line."
  exit 1
}

[[ $# != 1 ]] && usage

[[ ! -r ${file} ]] &&
  {
    echo "Unable to open input file @${file}@"
    exit 1
  }


#
# Basic syntax checking for the whole expression.
#
function isValidPoly()
{
  l_line=${1}

  # Character set check.
  [[ ! ${l_line} =~ "^.*[a-z0-9+-]$" ]] &&
    {
      echo "This line has invalid characters: @${l_line}@"
      return 1
    }

  [[ $l_line[1] == "+" ]] &&
    {
      echo "Line starts with a plus sign: @${l_line}@"
      return 1
    } 

  [[ $l_line[-1] == [+-] ]] &&
    {
      echo "Non-variable line ending: @${l_line}@"
      return 1
    }

  [[ ${l_line} =~ "\+\+" || ${l_line} =~ "\+-" || ${l_line} =~ "-\+" ||${l_line} =~ "--"  ]] &&
    {
      echo "Multi-operand in line: @${l_line}@"
      return 1
    }

  return 0
}


#
# Sort the characters of a string.
#
function sortString()
{
  [[ -z ${1} || ${1} == "" ]] && return
  echo ${${1/+/}/-/} | grep -o . | sort | tr -d '\n'
}


#
# List elements without signs
#
function listElements()
{
  [[ -z ${1} || ${1} == "" ]] && return 
  echo ${${1//+/\\n+}//-/\\n-}
}


#
# Remove leading numerals from a string.
#
function deNum()
{
  echo ${1//[0-9]/}
}


#
# Show only numerals of a string.
#
function onlyNum()
{
  echo ${1//[a-z-+]/}
}


#
# Check whether there are multiple numerals in a string. (e.g. 12a8b)
#
function disorderedCheck()
{
  [[ ${1} =~ "[0-9][a-z].*[0-9]" ]] && return 1
  return 0
}


#
# Update the value of a polynomial element.
#
function updatePoly()
{
  l_e=${1}
  l_sign=${2}
  l_val=${3}
  debug "Updating ${l_e} ${l_sign} ${l_val}"
  # ta["${tal}"]=$((ta["${tal}"] + 1))
  [[ ${l_sign} == "plus" ]]  && polynome[${l_e}]=$(( polynome[${l_e}] + ${l_val} ))
  [[ ${l_sign} == "minus" ]] && polynome[${l_e}]=$(( polynome[${l_e}] - ${l_val} ))
}


#
# Display the new polynomial in the expected format.
#
function printPoly()
{
  l_output=""
  echo ${(k)polynome} | tr ' ' '\n' | sort | while read l_e; do
    [[ $polynome[${l_e}] == 0 ]] && continue
    [[ ! -z ${l_output} && $polynome[${l_e}] -gt 0 ]] && l_output="${l_output}+"
    [[ $polynome[${l_e}] == 1 ]] &&
      {
        l_output="${l_output}${l_e}"
        continue
      }
    [[ $polynome[${l_e}] == -1 ]] &&
      {
        l_output="${l_output}-${l_e}"
        continue
      }    
    l_output="${l_output}$polynome[${l_e}]${l_e}"
  done
  [[ ${const} -gt 0 ]] && l_output=${l_output}+${const}
  [[ ${const} -lt 0 ]] && l_output=${l_output}${const}
  echo ${l_output}  
}


#
# Two arrays:
#  - elements: store input polynomial elements (list)
#  - polynome: associative array to store elements and values. (dict)
#
typeset -A elements
typeset -A polynome

cat ${file} | while read line ; do
  [[ -z ${line} || ${line} == "" || $line[1] == "#" ]] && continue

  elements=()
  polynome=()
  const=0
  echo "Processing input polynome: @${line}@"
  isValidPoly ${line}
  EC=$?
  debug "EC $EC"
  [[ $EC != 0 ]] &&
    {
      echo "Invalid polynomial found. Skipping."
      continue
    } 


  # Iterate across elements
  elements=$(listElements "${line}")
  for e in ${=elements}; do
    disorderedCheck ${e}
    [[ $? != 0 ]] &&
      {
        echo "\tInvalid multinumeral format: @${e}@"
        invalid=1
        break
      }
      
    # Fetch useful bits:
    debug "\tProcessing base element @${e}@"
    eOrdered=$(sortString $(deNum $e))
    debug "\t\teOrdered=@${eOrdered}@"
    eValue=${$(onlyNum $e):-1}
    debug "\t\teValue=@${eValue}@" 

    # Get the sign.
    [[ $e[1] =~ [a-z0-9+] ]] && 
      {
        eSign=plus 
      } || { 
        eSign=minus
      }
    debug "\t\teSign=@${eSign}@"
    echo "\tHandle element ${eOrdered} with value ${eSign} ${eValue}"

    # Handle constants.
    [[ ${eOrdered} == "" ]] &&
      {
        [[ ${eSign} == "plus" ]] && let const+=${eValue}
        [[ ${eSign} == "minus" ]] && let const-=${eValue}
        continue
      }
      
    # Elements appears for the first time. Add it to the array with zero value.
    [[ -z $polynome[${eOrdered}] ]] && 
      {
        debug "\t\tInitialising ${eOrdered}"
        polynome[${eOrdered}]=0
      }
    # Update with new value.
    updatePoly ${eOrdered} ${eSign} ${eValue}
  done

[[ ${invalid} == 1 ]] &&
  {
    unset invalid
    continue
  }

# "Pretty print" the polynomial.
printPoly

done
