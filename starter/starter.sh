#!/bin/zsh 
export LD_LIBRARY_PATH=/home/bacs/lib:${LD_LIBRARY_PATH}
export RUBYLIB=/home/bacs/lib/ruby:${RUBYLIB}

cd /home/bacs/balloon/operation/onboard



while :
do
id=`date "+%Y%m%d%H%M%S"`
./test.rb  &> output_${id}.log
end_status=$?
echo ${end_status}
if [ "${end_status}" -eq "111" ]
then
exit
fi
done

