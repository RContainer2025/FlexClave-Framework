PARALLELISM=`getconf _NPROCESSORS_ONLN`

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pushd $DIR/..
TOP_DIR=`pwd`
popd

PLATFORM=aemfvp-a-rme
PLATDIR=${TOP_DIR}/output/aemfvp-a-rme
OUTDIR=${PLATDIR}/components
LINUX_OUT_DIR=out/aemfvp-a-rme

mkdir -p $PLATDIR

source $DIR/configs/aemfvp-a-rme/aemfvp-a-rme

case "$CMD" in
	prepare) do_prepare
	;;
	
	build) do_build
	;;

	clean) do_clean
	;;

	package) do_package
	;;
	
	ignore) echo "build-all.sh ignore"
	;;

	*) exit 1
	;;
esac

