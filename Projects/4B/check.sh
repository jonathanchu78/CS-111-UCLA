echo | ./lab4b --badarg &> /dev/null; \
if [[ $? -ne 1 ]]; then \
echo "Test FAILED: bad argument returns wrong exit code"; \
else \
echo "Test PASSED: bad argument returns correct exit code"; \
fi
# regular args test
./lab4b --period=2 --scale="C" --log="LOGFILE" <<-EOF
SCALE=F
STOP
OFF
EOF
ret=$?
if [ $ret -ne 0 ]
then
	echo "Test FAILED: bad exit code for valid args & commands"
else
	echo "Test PASSED: correct exit code for valid args & commands"
fi
# created file test
if [ ! -s LOGFILE ]
then
	echo "Test FAILED: LOGFILE not created"
else
	echo "Test PASSED: LOGFILE created"
fi
# logged command test
grep "STOP" LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then
	echo "Test FAILED: STOP command not found in LOGFILE"
else
	echo "Test PASSED: STOP command found in LOGFILE"
fi
# SHUTDOWN logged test
grep "SHUTDOWN" LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then 
	echo "Test FAILED: SHUTDOWN not logged"
else
	echo "Test PASSED: SHUTDOWN logged"
fi
# temperature correct format test
egrep '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then 
	echo "Test FAILED: Temp not logged properly"
else
	echo "Test PASSED: Temp logged properly"
fi
rm -f LOGFILE
# test for invalid command during execution
./lab4b --period=2 --scale="C" --log="LOGFILE" <<-EOF
SCALE=F
STOP
BADARG
EOF
ret=$?
if [ $ret -eq 0 ]
then
	echo "Test FAILED: Bad command should result in nonzero exit code"
else
	echo "Test PASSED: Bad command results in nonzero exit code"
fi
rm -f LOGFILE