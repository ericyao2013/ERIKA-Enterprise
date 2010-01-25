#
# MODISTARC TESTCASES COMPILATION
#

echo "Executing startup script in progress..."
. ./s12xs_startup_scripts 
echo "Executing startup script in progress...Done!!!"

if [ "$1"=="clean" ]											# Clean
then
echo "Clean in progress..."
make clean
echo "Clean in progress...Done!!!"
elif [ "$1"=="all" ] 											# Make all
then
echo "Modistarc test compilation in progress..."
echo "All tests will be compiled!"
make ARCH=s12xs
echo "Modistarc test compilation in progress...Done!!!"
elif [ "$1"=="" ] 												# Make all if no args
echo "Modistarc test compilation in progress..."
echo "No arguments found. All tests will be compiled!"
make ARCH=s12xs
echo "Modistarc test compilation in progress...Done!!!"
else															# Make only one test...
echo "Modistarc test compilation in progress..."
echo "One argument found. Only $1 will be compiled."
make ARCH=s12xs DIRS="$1"
echo "Modistarc test compilation in progress...Done!!!"
fi


