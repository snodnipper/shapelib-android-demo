/*
 ============================================================================
 Name        : WGS84 lat lng to BNG Eastings and Northings converter (demo)
 Author      : snodnipper
 Description : A C translation of a small subset of jcoord to, specifically,
               transform WGS84 latitude and longitude values into
               British National Grid (BNG) Eastings and Northings.
 License     : GPL (inherited)
 ============================================================================

 --------------------------------------------------------------------------
 Jcoord
 readme.txt

 (c) 2006 Jonathan Stott

 Created on 11-Feb-2006

 1.0 - 11 Feb 2006
  - Initial version created from PHPcoord v2.2
--------------------------------------------------------------------------

Jcoord is a collection of Java classes that provide functions for handling
various co-ordinate systems and converting between them. Currently, OSGB
(Ordnance Survey of Great Britain) grid references, UTM (Universal Transverse
Mercator) references and latitude/longitude are supported. A function is
also provided to find the surface distance between two points of latitude
and longitude.

When using the OSGB conversions, the majority of applications use the
WGS84 datum rather than the OSGB36 datum. Conversions between the two
data were added in v1.1 - the conversions should be accurate to within
5m or so. If accuracy is not important (i.e. to within 200m or so),
then it isn't necessary to perform the conversions.

Examples of how to use the classes in Jcoord can be found in the
uk.me.jstott.jcoord.Test class

See http://www.jstott.me.uk/jcoord/ for latest releases and information.


DISCLAIMER

Accuracy of the co-ordinate conversions contained within the Jcoord
package is not guaranteed. Use of the conversions is entirely at your
own risk and I cannot be held responsible for any consequences of
errors created by the conversions. I do not recommend using the package
for mission-critical applications.


LICENSING

This software product is available under the GNU General Public License
(GPL). Terms of the GPL can be read at http://www.jstott.me.uk/gpl/.
Any commercial use requires the purchase of a license - contact me at
jcoord@jstott.me.uk for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

struct OSRef {
	double easting, northing;
};
typedef struct OSRef OSRef;

struct RefEll {
    double maj, min, ecc;
};
typedef struct RefEll RefEll;

struct Wgs84LatLng {
    double lat, lng;
};
typedef struct Wgs84LatLng Wgs84LatLng;

struct Osgb36LatLng {
    double lat, lng;
};
typedef struct Osgb36LatLng Osgb36LatLng;

double getEccentricity(double maj, double min) {
    return ((maj * maj) - (min * min)) / (maj * maj);
}

double sinSquared(double value) {
	return pow(sin(value), 2);
}

double tanSquared(double value) {
	return pow(tan(value), 2);
}

double toDegrees(double value) {
	return value / M_PI * 180.0;
}

double toRadians(double value) {
	return value * M_PI / 180.0;
}

OSRef Osgb36ToOsref(Osgb36LatLng osgb36LatLng) {
	RefEll airy1830 = {6377563.396, 6356256.909, getEccentricity(6377563.396, 6356256.909)};
	double OSGB_F0 = 0.9996012717;
	double N0 = -100000.0;
	double E0 = 400000.0;
	double phi0 = toRadians(49.0);
	double lambda0 = toRadians(-2.0);
	double a = airy1830.maj;
	double b = airy1830.min;
	double eSquared = airy1830.ecc;
	double phi = toRadians(osgb36LatLng.lat);
	double lambda = toRadians(osgb36LatLng.lng);
	double E = 0.0;
	double N = 0.0;
	double n = (a - b) / (a + b);
	double v = a * OSGB_F0 * pow(1.0 - eSquared * sinSquared(phi), -0.5);
	double rho = a * OSGB_F0 * (1.0 - eSquared) * pow(1.0 - eSquared * sinSquared(phi), -1.5);
	double etaSquared = (v / rho) - 1.0;
	double M = (b * OSGB_F0) * (((1 + n + ((5.0 / 4.0) * n * n) + ((5.0 / 4.0) * n * n * n)) * (phi - phi0)) - (((3 * n) + (3 * n * n) + ((21.0 / 8.0) * n * n * n)) * sin(phi - phi0) * cos(phi + phi0)) + ((((15.0 / 8.0) * n * n) + ((15.0 / 8.0) * n * n * n)) * sin(2.0 * (phi - phi0)) * cos(2.0 * (phi + phi0))) - (((35.0 / 24.0) * n * n * n) * sin(3.0 * (phi - phi0)) * cos(3.0 * (phi + phi0))));
	double I = M + N0;
	double II = (v / 2.0) * sin(phi) * cos(phi);
	double III = (v / 24.0) * sin(phi) * pow(cos(phi), 3.0) * (5.0 - tanSquared(phi) + (9.0 * etaSquared));
	double IIIA = (v / 720.0) * sin(phi) * pow(cos(phi), 5.0) * (61.0 - (58.0 * tanSquared(phi)) + pow(tan(phi), 4.0));
	double IV = v * cos(phi);
	double V = (v / 6.0) * pow(cos(phi), 3.0) * ((v / rho) - tanSquared(phi));
	double VI = (v / 120.0) * pow(cos(phi), 5.0) * (5.0 - (18.0 * tanSquared(phi)) + (pow(tan(phi), 4.0)) + (14 * etaSquared) - (58 * tanSquared(phi) * etaSquared));

	N = I + (II * pow(lambda - lambda0, 2.0)) + (III * pow(lambda - lambda0, 4.0)) + (IIIA * pow(lambda - lambda0, 6.0));
	E = E0 + (IV * (lambda - lambda0)) + (V * pow(lambda - lambda0, 3.0)) + (VI * pow(lambda - lambda0, 5.0));

	OSRef osref = {E, N};
	return osref;
}

Osgb36LatLng wgs84ToOsgb36(Wgs84LatLng wgs84LatLng) {
	RefEll wgs84 = (RefEll) {6378137.000, 6356752.3141, getEccentricity(6378137.000, 6356752.3141)};
	double a = wgs84.maj;
	double eSquared = wgs84.ecc;
	double phi = toRadians(wgs84LatLng.lat);
	double lambda = toRadians(wgs84LatLng.lng);
	double v = a / (sqrt(1 - eSquared * sinSquared(phi)));
	double H = 0; // height
	double x = (v + H) * cos(phi) * cos(lambda);
	double y = (v + H) * cos(phi) * sin(lambda);
	double z = ((1 - eSquared) * v + H) * sin(phi);

	double tx = -446.448;
	double ty = 124.157;
	double tz = -542.060;
	double s = 0.0000204894;
	double rx = toRadians(-0.00004172222);
	double ry = toRadians(-0.00006861111);
	double rz = toRadians(-0.00023391666);

	double xB = tx + (x * (1 + s)) + (-rx * y) + (ry * z);
	double yB = ty + (rz * x) + (y * (1 + s)) + (-rx * z);
	double zB = tz + (-ry * x) + (rx * y) + (z * (1 + s));

	RefEll airy1830 = {6377563.396, 6356256.909, getEccentricity(6377563.396, 6356256.909)};
	a = airy1830.maj;
	eSquared = airy1830.ecc;

	double lambdaB = toDegrees(atan(yB / xB));
	double p = sqrt((xB * xB) + (yB * yB));
	double phiN = atan(zB / (p * (1 - eSquared)));
	int i;
	for (i = 1; i < 10; i++)
	{
		v = a / (sqrt(1 - eSquared * sinSquared(phiN)));
		double phiN1 = atan((zB + (eSquared * v * sin(phiN))) / p);
		phiN = phiN1;
	}

	double phiB = toDegrees(phiN);

	Osgb36LatLng Osgb36LatLng = {phiB, lambdaB};

	return Osgb36LatLng;
}

void check(Wgs84LatLng wgs84LatLng, double expectedEasting, double expectedNorthing) {
	Osgb36LatLng osgb36LatLng = wgs84ToOsgb36(wgs84LatLng);
	OSRef osref = Osgb36ToOsref(osgb36LatLng);
	printf("\nWGS84 Lat,Lng - %f,%f", wgs84LatLng.lat, wgs84LatLng.lng);
	printf("\nOSGB36 Lat,Lng - %f,%f", osgb36LatLng.lat, osgb36LatLng.lng);
	printf("\nEasting: %f, Northing: %f", osref.easting, osref.northing);

	const double EPS = 0.001;
	double difference = abs(osref.easting - expectedEasting);

	if (difference < EPS) {
		printf("\nAcceptable values:\nEastings: %f = %f\nNorthings: %f = %f", osref.easting, expectedEasting, osref.northing, expectedNorthing);
	} else {
		printf("\nUNACCEPTABLE values: %f %f - difference: %f", osref.easting, expectedEasting, difference);
	}
}

int main(void) {
	puts("!!!Test from GPS lat lon values to BNG!!!");

	// Test 1 (London Eye)
	check((Wgs84LatLng){51.5033, -0.1196}, 530608.5016051171, 179938.7009563997);

	// Test 2 (Big Ben)
	check((Wgs84LatLng){51.500773300000000000, -0.124640199999930700}, 530265.9038040269, 179648.7480692718);

	// Test 3 (Penzance)
	check((Wgs84LatLng){50.119, -5.537}, 147256.0577745991, 30315.966031762255);

	// Test 4 (Arthur's Seat, Edinburgh)
	check((Wgs84LatLng){55.944152, -3.162178}, 327510.18723728694, 672954.8020750559);

	// Test 5 (Mt Snowdon)
	check((Wgs84LatLng){53.068497, -4.076231}, 260984.62345337166, 354376.964935836);

	return EXIT_SUCCESS;
}
