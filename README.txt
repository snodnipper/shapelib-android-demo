# Build the native library (only) #
> From within 'shapelib-android-demo/shapelib-android-demo' you can execute
  'ndk-build clean; ndk-build'

# Build / install / run the program #
./shapelib-android-demo/shapelib-android-demo/$ mvn clean install android clean install android:ndk-build android:deploy

# Execution #
Running the activity causes the /sdcard/trees shapefile to be written.

# Maven Android Project Definition #

  mvn archetype:generate \
    -DarchetypeArtifactId=android-with-test \
    -DarchetypeGroupId=de.akquinet.android.archetypes \
    -DarchetypeVersion=1.0.10 \
    -DgroupId=org.maptools.shapelib.android \
    -DartifactId=shapelib-android-demo \
    -Dpackage=org.maptools.shapelib.android