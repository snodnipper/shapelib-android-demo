package org.maptools.shapelib.android.util;

public final class TestUtils {

    public static final String DEMO_SHAPEFILE = "/sdcard/trees";

    /* this is used to load the native code at application startup. The library has already been
     * unpacked into /data/data/org.maptools.shapelib.android/lib/libshapelib-android-demo.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("shapelib-android-demo");
    }

    private TestUtils(){}

    public static void createDemoShapefile() {
        createShapefile(DEMO_SHAPEFILE, new DataAdapter<AppleTree>() {
            @Override
            public int size() {
                return 5;
            }

            @Override
            public AppleTree get(int index) {
                return new AppleTree(50D + index * 0.01, index * 0.01, "Apple Tree " + index,
                        index * 2, 1 + (index * 0.5));
            }
        });
    }

    public static void appendDemoShapefile() {
        updateShapefile(DEMO_SHAPEFILE, new DataAdapter<AppleTree>() {
            @Override
            public int size() {
                return 5;
            }

            @Override
            public AppleTree get(int index) {
                return new AppleTree(50.1D + index * 0.01, index * 0.01, "Append Apple Tree " + index,
                        50, 4.5);
            }
        });
    }

    private static native boolean createShapefile(String filePath, DataAdapter<AppleTree> adapter);

    private static native boolean updateShapefile(String filePath, DataAdapter<AppleTree> adapter);

    private interface DataAdapter<E> {

        int size();

        E get(int index);
    }

    private static class AppleTree {
        private final double mLatitude;
        private final double mLongitude;
        private final String mName;
        private final int mApples;
        private final double mHeight;

        public AppleTree(double latitude, double longitude, String name, int apples,
                         double height) {
            mLatitude = latitude;
            mLongitude = longitude;
            mName = name;
            mApples = apples;
            mHeight = height;
        }

        public double getLatitude() {
            return mLatitude;
        }

        public double getLongitude() {
            return mLongitude;
        }

        public String getName() {
            return mName;
        }

        public int getApples() {
            return mApples;
        }

        public double getHeight() {
            return mHeight;
        }
    }
}
