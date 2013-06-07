/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "shapelib/shapefil.h"

void process(JNIEnv * env, SHPHandle hSHP, DBFHandle hDBF, jstring file, jobject listObject);
int writePoint(JNIEnv * env, SHPHandle hSHP, DBFHandle hDBF, int index, double latitude, double longitude, jstring name, jdouble height, int apples);

jboolean Java_org_maptools_shapelib_android_util_TestUtils_updateShapefile(JNIEnv * env, jclass clazz,
                                                                jstring file, jobject listObject) {
    // open an existing shapefile and dbf (e.g. /sdcard/foo/bar)
    const char *fileStr = (*env)->GetStringUTFChars(env, file, 0);
    SHPHandle hSHP = SHPOpen(fileStr, "rb+");
    DBFHandle hDBF = DBFOpen(fileStr, "rb+");
    (*env)->ReleaseStringUTFChars(env, file, fileStr);

    process(env, hSHP, hDBF, file, listObject);

    SHPClose(hSHP);
    DBFClose(hDBF);
}

jboolean Java_org_maptools_shapelib_android_util_TestUtils_createShapefile(JNIEnv * env, jclass clazz,
                                                                jstring file, jobject listObject) {
    // create a shapefile and dbf (e.g. /sdcard/foo/bar)
    const char *fileStr = (*env)->GetStringUTFChars(env, file, 0);
    SHPHandle hSHP = SHPCreate(fileStr, SHPT_POINT);
    DBFHandle hDBF = DBFCreate(fileStr);

    // define the shapefile attributes
    DBFAddField(hDBF, "name", FTString, 25, 0);
    DBFAddField(hDBF, "height", FTDouble, 8, 8);
    DBFAddField(hDBF, "apples", FTInteger, 1, 0);

    process(env, hSHP, hDBF, file, listObject);

    SHPClose(hSHP);
    DBFClose(hDBF);
}

void process(JNIEnv * env, SHPHandle hSHP, DBFHandle hDBF, jstring file, jobject listObject) {
    jclass classAdapter = (*env)->FindClass(env, "org/maptools/shapelib/android/util/TestUtils$DataAdapter");
    jmethodID methodIdSize = (*env)->GetMethodID(env, classAdapter, "size", "()I");
    jmethodID methodIdGet = (*env)->GetMethodID(env, classAdapter, "get", "(I)Ljava/lang/Object;");    // see: http://docs.oracle.com/javase/6/docs/technotes/guides/jni/spec/types.html#wp16437

    int totalItems = (*env)->CallIntMethod(env, listObject, methodIdSize);

    // Apple tree
    jclass classAppleTree = (*env)->FindClass(env, "org/maptools/shapelib/android/util/TestUtils$AppleTree");
    jmethodID methodIdGetLatitude = (*env)->GetMethodID(env, classAppleTree, "getLatitude", "()D");
    jmethodID methodIdGetLongitude = (*env)->GetMethodID(env, classAppleTree, "getLongitude", "()D");
    jmethodID methodIdGetName = (*env)->GetMethodID(env, classAppleTree, "getName", "()Ljava/lang/String;");
    jmethodID methodIdGetHeight = (*env)->GetMethodID(env, classAppleTree, "getHeight", "()D");
    jmethodID methodIdGetApples = (*env)->GetMethodID(env, classAppleTree, "getApples", "()I");

    // Let us print out those items
   	const int recordCount = DBFGetRecordCount(hDBF);
	const int end = totalItems + recordCount;
    int i;
    for (i = recordCount; i < end; i++) {
        jobject tree = (*env)->CallObjectMethod(env, listObject, methodIdGet, i);
        jstring name = (*env)->CallObjectMethod(env, tree, methodIdGetName);
        jint apples = (*env)->CallIntMethod(env, tree, methodIdGetApples);
        jdouble height = (*env)->CallDoubleMethod(env, tree, methodIdGetHeight);
        jdouble latitude = (*env)->CallDoubleMethod(env, tree, methodIdGetLatitude);
        jdouble longitude = (*env)->CallDoubleMethod(env, tree, methodIdGetLongitude);

        writePoint(env, hSHP, hDBF, i, latitude, longitude, name, height, apples);

        // free reference and avoid a "JNI local reference table" overflow
        (*env)->DeleteLocalRef(env,  tree );
        (*env)->DeleteLocalRef(env,  name );
    }
}

int writePoint(JNIEnv * env, SHPHandle hSHP, DBFHandle hDBF, int index, double latitude, double longitude, jstring name, jdouble height, int apples)
{
    double *x = malloc(sizeof(*x));
    double *y = malloc(sizeof(*y));

    x[0] = longitude;
    y[0] = latitude;
    SHPObject *oSHP = SHPCreateSimpleObject(SHPT_POINT, 1, x, y, NULL);
    SHPWriteObject(hSHP, -1, oSHP);
    const char *nativeName = (*env)->GetStringUTFChars(env, name, 0);
    DBFWriteStringAttribute(hDBF, index, 0, nativeName);
    (*env)->ReleaseStringUTFChars(env, name, nativeName);
    DBFWriteDoubleAttribute(hDBF, index, 1, height);
    DBFWriteIntegerAttribute(hDBF, index, 2, apples);

    SHPDestroyObject(oSHP);

    free(x);
    free(y);

    return 0;
}
