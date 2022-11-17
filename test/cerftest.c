/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File cerftest.c
 *   Test the complex error functions.
 *
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013 Forschungszentrum Jülich GmbH
 *
 * Licence:
 *   ../LICENSE
 *
 * Authors:
 *   Steven G. Johnson, Massachusetts Institute of Technology, 2012, core author
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013, package maintainer
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 *
 * Revision history:
 *   ../CHANGELOG
 */

#include "cerf.h"
#include "testtool.h"

int main()
{
    result_t result = {0, 0};

    /***************   cerf   *****************/

    /* reference values computed with Maple */

    ZTEST(
        result, 1e-13, cerf(C(1, 2)),
        C(-0.5366435657785650339917955593141927494421, -5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerf(C(-1, 2)),
        C(0.5366435657785650339917955593141927494421, -5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerf(C(1, -2)),
        C(-0.5366435657785650339917955593141927494421, 5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerf(C(-1, -2)),
        C(0.5366435657785650339917955593141927494421, 5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerf(C(9, -28)),
        C(0.3359473673830576996788000505817956637777e304,
          -0.1999896139679880888755589794455069208455e304));
    ZTEST(
        result, 1e-13, cerf(C(21, -33)),
        C(0.3584459971462946066523939204836760283645e278,
          0.3818954885257184373734213077678011282505e280));
    ZTEST(
        result, 1e-13, cerf(C(1e3, 1e3)),
        C(0.9996020422657148639102150147542224526887,
          0.00002801044116908227889681753993542916894856));
    ZTEST(result, 1e-13, cerf(C(-3001, -1000)), C(-1, 0));
    ZTEST(result, 1e-13, cerf(C(1e160, -1e159)), C(1, 0));
    ZTEST(
        result, 1e-13, cerf(C(5.1e-3, 1e-8)),
        C(0.005754683859034800134412990541076554934877,
          0.1128349818335058741511924929801267822634e-7));
    ZTEST(
        result, 1e-13, cerf(C(-4.9e-3, 4.95e-3)),
        C(-0.005529149142341821193633460286828381876955,
          0.005585388387864706679609092447916333443570));
    ZTEST(
        result, 1e-13, cerf(C(4.9e-3, 0.5)),
        C(0.007099365669981359632319829148438283865814,
          0.6149347012854211635026981277569074001219));
    ZTEST(
        result, 1e-13, cerf(C(4.9e-4, -0.5e1)),
        C(0.3981176338702323417718189922039863062440e8,
          -0.8298176341665249121085423917575122140650e10));
    ZTEST(result, 1e-13, cerf(C(-4.9e-5, -0.5e2)), C(-Inf, -Inf));
    ZTEST(
        result, 1e-13, cerf(C(5.1e-3, 0.5)),
        C(0.007389128308257135427153919483147229573895,
          0.6149332524601658796226417164791221815139));
    ZTEST(
        result, 1e-13, cerf(C(5.1e-4, -0.5e1)),
        C(0.4143671923267934479245651547534414976991e8,
          -0.8298168216818314211557046346850921446950e10));
    ZTEST(result, 1e-13, cerf(C(-5.1e-5, -0.5e2)), C(-Inf, -Inf));
    ZTEST(
        result, 1e-13, cerf(C(1e-6, 2e-6)),
        C(0.1128379167099649964175513742247082845155e-5,
          0.2256758334191777400570377193451519478895e-5));
    ZTEST(result, 1e-13, cerf(C(0, 2e-6)), C(0, 0.2256758334194034158904576117253481476197e-5));
    ZTEST(result, 1e-13, cerf(C(0, 2)), C(0, 18.56480241457555259870429191324101719886));
    ZTEST(result, 1e-13, cerf(C(0, 20)), C(0, 0.1474797539628786202447733153131835124599e173));
    ZTEST(result, 1e-13, cerf(C(0, 200)), C(0, Inf));
    ZTEST(result, 1e-13, cerf(C(Inf, 0)), C(1, 0));
    ZTEST(result, 1e-13, cerf(C(-Inf, 0)), C(-1, 0));
    ZTEST(result, 1e-13, cerf(C(0, Inf)), C(0, Inf));
    ZTEST(result, 1e-13, cerf(C(0, -Inf)), C(0, -Inf));
    ZTEST(result, 1e-13, cerf(C(Inf, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerf(C(Inf, -Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerf(C(NaN, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerf(C(NaN, 0)), C(NaN, 0));
    ZTEST(result, 1e-13, cerf(C(0, NaN)), C(0, NaN));
    ZTEST(result, 1e-13, cerf(C(NaN, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerf(C(Inf, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerf(C(1e-3, NaN)), C(NaN, NaN));
    ZTEST(
        result, 1e-13, cerf(C(7e-2, 7e-2)),
        C(0.07924380404615782687930591956705225541145,
          0.07872776218046681145537914954027729115247));
    ZTEST(
        result, 1e-13, cerf(C(7e-2, -7e-4)),
        C(0.07885775828512276968931773651224684454495,
          -0.0007860046704118224342390725280161272277506));
    ZTEST(
        result, 1e-13, cerf(C(-9e-2, 7e-4)),
        C(-0.1012806432747198859687963080684978759881,
          0.0007834934747022035607566216654982820299469));
    ZTEST(
        result, 1e-13, cerf(C(-9e-2, 9e-2)),
        C(-0.1020998418798097910247132140051062512527, 0.1010030778892310851309082083238896270340));
    ZTEST(
        result, 1e-13, cerf(C(-7e-4, 9e-2)),
        C(-0.0007962891763147907785684591823889484764272,
          0.1018289385936278171741809237435404896152));
    ZTEST(
        result, 1e-13, cerf(C(7e-2, 0.9e-2)),
        C(0.07886408666470478681566329888615410479530,
          0.01010604288780868961492224347707949372245));
    ZTEST(
        result, 1e-13, cerf(C(7e-2, 1.1e-2)),
        C(0.07886723099940260286824654364807981336591,
          0.01235199327873258197931147306290916629654));

    /***************   cerfc   *****************/

    /* reference values computed with Maple */

    ZTEST(
        result, 1e-13, cerfc(C(1, 2)),
        C(1.536643565778565033991795559314192749442, 5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerfc(C(-1, 2)),
        C(0.4633564342214349660082044406858072505579, 5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerfc(C(1, -2)),
        C(1.536643565778565033991795559314192749442, -5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerfc(C(-1, -2)),
        C(0.4633564342214349660082044406858072505579, -5.049143703447034669543036958614140565553));
    ZTEST(
        result, 1e-13, cerfc(C(9, -28)),
        C(-0.3359473673830576996788000505817956637777e304,
          0.1999896139679880888755589794455069208455e304));
    ZTEST(
        result, 1e-13, cerfc(C(21, -33)),
        C(-0.3584459971462946066523939204836760283645e278,
          -0.3818954885257184373734213077678011282505e280));
    ZTEST(
        result, 1e-13, cerfc(C(1e3, 1e3)),
        C(0.0003979577342851360897849852457775473112748,
          -0.00002801044116908227889681753993542916894856));
    ZTEST(result, 1e-13, cerfc(C(-3001, -1000)), C(2, 0));
    ZTEST(result, 1e-13, cerfc(C(1e160, -1e159)), C(0, 0));
    ZTEST(
        result, 1e-13, cerfc(C(5.1e-3, 1e-8)),
        C(0.9942453161409651998655870094589234450651,
          -0.1128349818335058741511924929801267822634e-7));
    ZTEST(result, 1e-13, cerfc(C(0, 2e-6)), C(1, -0.2256758334194034158904576117253481476197e-5));
    ZTEST(result, 1e-13, cerfc(C(0, 2)), C(1, -18.56480241457555259870429191324101719886));
    ZTEST(result, 1e-13, cerfc(C(0, 20)), C(1, -0.1474797539628786202447733153131835124599e173));
    ZTEST(result, 1e-13, cerfc(C(0, 200)), C(1, -Inf));
    ZTEST(result, 1e-13, cerfc(C(2e-6, 0)), C(0.9999977432416658119838633199332831406314, 0));
    ZTEST(result, 1e-13, cerfc(C(2, 0)), C(0.004677734981047265837930743632747071389108, 0));
    ZTEST(result, 1e-13, cerfc(C(20, 0)), C(0.5395865611607900928934999167905345604088e-175, 0));
    ZTEST(result, 1e-13, cerfc(C(200, 0)), C(0, 0));
    ZTEST(result, 1e-13, cerfc(C(Inf, 0)), C(0, 0));
    ZTEST(result, 1e-13, cerfc(C(-Inf, 0)), C(2, 0));
    ZTEST(result, 1e-13, cerfc(C(0, Inf)), C(1, -Inf));
    ZTEST(result, 1e-13, cerfc(C(0, -Inf)), C(1, Inf));
    ZTEST(result, 1e-13, cerfc(C(Inf, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerfc(C(Inf, -Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerfc(C(NaN, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerfc(C(NaN, 0)), C(NaN, 0));
    ZTEST(result, 1e-13, cerfc(C(0, NaN)), C(1, NaN));
    ZTEST(result, 1e-13, cerfc(C(NaN, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerfc(C(Inf, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, cerfc(C(88, 0)), C(0, 0));

    /***************   cerfi   *****************/

    // since cerfi just calls through to cerf, one test is enough
    // to make sure we didn't screw up the signs or something

    ZTEST(
        result, 1e-15, cerfi(C(1.234, 0.5678)),
        C(1.081032284405373149432716643834106923212, 1.926775520840916645838949402886591180834));

    /***************   cerfcx   *****************/

    // since cerfcx just calls through to w_of_z, one test is enough
    // to make sure we didn't screw up the signs or something

    // reference value computed with Maple

    ZTEST(
        result, 1e-13, cerfcx(C(1.234, 0.5678)),
        C(0.3382187479799972294747793561190487832579, -0.1116077470811648467464927471872945833154));

    /***************   erfcx   *****************/

    // Not checked against external reference; just status quo from libcerf-2.2.
    // Further tests, namely erfcx vs Re(cerfcx), are in realtest.c.

    // change of algorithm at -26.7
    RTEST(result, 1e-13, erfcx(-26.7000000001), Inf);
    RTEST(result, 1e-13, erfcx(-26.6999999999), Inf);

    RTEST(result, 1e-13, erfcx(-19.5), 2.76380080745855e+165);
    RTEST(result, 1e-13, erfcx(-12.3), 1.01260988545376e+66);

    // change of algorithm at -6.1
    RTEST(result, 1e-13, erfcx(-6.10000000003), 2.89152976642548e+16);
    RTEST(result, 1e-13, erfcx(-6.10000000001), 2.89152976571995e+16);
    RTEST(result, 1e-13, erfcx(-6.09999999999), 2.89152976501441e+16);
    RTEST(result, 1e-13, erfcx(-6.09999999997), 2.89152976430888e+16);

    RTEST(result, 1e-13, erfcx(-3), 16205.9888539996);
    RTEST(result, 1e-13, erfcx(-1), 5.00898008076228);

    // change of algorithm at 0
    RTEST(result, 1e-13, erfcx(-1e-1), 1.12364335419921);
    RTEST(result, 1e-13, erfcx(-1e-4), 1.00011284791746);
    RTEST(result, 1e-13, erfcx(-1e-8), 1.00000001128379);
    RTEST(result, 1e-13, erfcx(-1e-12), 1.00000000000113);
    RTEST(result, 1e-13, erfcx(-1e-20), 1);
    RTEST(result, 1e-13, erfcx(0), 1);
    RTEST(result, 1e-13, erfcx(1e-20), 1);
    RTEST(result, 1e-13, erfcx(1e-12), 0.999999999998872);
    RTEST(result, 1e-13, erfcx(1e-8), 0.999999988716208);
    RTEST(result, 1e-13, erfcx(1e-4), 0.999887172082539);
    RTEST(result, 1e-13, erfcx(1e-1), 0.896456979969127);

    RTEST(result, 1e-13, erfcx(1), 0.427583576155807);
    RTEST(result, 1e-13, erfcx(3), 0.17900115118139);
    RTEST(result, 1e-13, erfcx(6), 0.0927765678005384);
    RTEST(result, 1e-13, erfcx(10), 0.0561409927438226);
    RTEST(result, 1e-13, erfcx(20), 0.0281743487410513);
    RTEST(result, 1e-13, erfcx(30), 0.0187958888614168);

    // change of algorithm at 50
    RTEST(result, 1e-13, erfcx(49.99999999999), 0.011281536265326);
    RTEST(result, 1e-13, erfcx(50.00000000001), 0.0112815362653215);

    RTEST(result, 1e-13, erfcx(1e2), 0.00564161378298943);
    RTEST(result, 1e-13, erfcx(1e3), 0.000564189301453388);
    RTEST(result, 1e-13, erfcx(1e4), 5.64189580726808e-05);
    RTEST(result, 1e-13, erfcx(1e5), 5.64189583519547e-06);
    RTEST(result, 1e-13, erfcx(1e6), 5.64189583547474e-07);
    RTEST(result, 1e-13, erfcx(1e7), 5.64189583547753e-08);

    // change of algorithm at 5e7
    RTEST(result, 1e-13, erfcx(5e7), 1.12837916709551e-08);
    RTEST(result, 1e-13, erfcx(5.000000000001e7), 1.12837916709529e-08);

    RTEST(result, 1e-13, erfcx(1e8), 5.64189583547756e-09);
    RTEST(result, 1e-13, erfcx(1e20), 5.64189583547756e-21);
    RTEST(result, 1e-13, erfcx(1e40), 5.64189583547756e-41);
    RTEST(result, 1e-13, erfcx(1e80), 5.64189583547756e-81);
    RTEST(result, 1e-13, erfcx(1e120), 5.64189583547756e-121);
    RTEST(result, 1e-13, erfcx(1e160), 5.64189583547756e-161);

    /***************   terminate   *****************/

    printf("%i/%i tests failed\n", result.failed, result.total);
    return result.failed;
}
