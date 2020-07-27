/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File test1.c:
 *   Test implementation of Faddeeva, Dawson, and error functions
 *   by checking against values computed using Maple and Wolfram Alfa.
 *
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013 Forschungszentrum Jülich GmbH
 *
 * Licence:
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
 *
 * More information:
 *   man 3 faddeeva
 */

#include "cerf.h"
#include "testtool.h"

#include <float.h>
#include <math.h>
#include <stdio.h>

#include "defs.h" // defines _cerf_cmplx, CMPLX, NaN

const double errBound = 1e-13;

/******************************************************************************/
/*  Auxiliary routines                                                        */
/******************************************************************************/

// compute relative error |b-a|/|a|, handling case of NaN and Inf,
static double relerr(double a, double b)
{
    if (isnan(a) || isnan(b) || isinf(a) || isinf(b)) {
        if ((isnan(a) && !isnan(b)) || (!isnan(a) && isnan(b)) || (isinf(a) && !isinf(b))
            || (!isinf(a) && isinf(b)) || (isinf(a) && isinf(b) && a * b < 0))
            return Inf; // "infinite" error
        return 0; // matching infinity/nan results counted as zero error
    }
    if (a == 0)
        return b == 0 ? 0 : Inf;
    else
        return fabs((b - a) / a);
}

// For testing the complex Dawson and error functions.

void zTest(
    int* fail, const char* fctName, _cerf_cmplx (*F)(_cerf_cmplx), int nTst, _cerf_cmplx* w,
    _cerf_cmplx* z)
{
    printf("############# %s(z) tests #############\n", fctName);
    for (int i = 0; i < nTst; ++i) {
        _cerf_cmplx fw = F(z[i]);
        double re_err = relerr(creal(w[i]), creal(fw));
        double im_err = relerr(cimag(w[i]), cimag(fw));
        int ok = re_err <= errBound && im_err <= errBound;
        printf(ok ? "   " : "ERR");
        if (!ok)
            ++(*fail);
        printf(
            " %s(%g%+gi) = %g%+gi (vs. %g%+gi), "
            "re/im rel. err. = %0.2g/%0.2g)\n",
            fctName, creal(z[i]), cimag(z[i]), creal(fw), cimag(fw), creal(w[i]), cimag(w[i]),
            re_err, im_err);
    }
}

// For testing the Dawson and error functions for the special case of a real
// argument

void xTest(
    int* fail, const char* fctName, _cerf_cmplx (*F)(_cerf_cmplx), double (*FRE)(double),
    double isc)
{
    printf("############# %s(x) tests #############\n", fctName);
    double errmax = 0;
    for (int i = 0; i < 10000; ++i) {
        double x = pow(10., -300. + i * 600. / (10000 - 1));
        double re_err = relerr(FRE(x), creal(F(C(x, x * isc))));
        if (re_err > errmax)
            errmax = re_err;
        re_err = relerr(FRE(-x), creal(F(C(-x, x * isc))));
        if (re_err > errmax)
            errmax = re_err;
    }
    if (errmax > errBound) {
        printf("FAILURE -- relative error %g too large!\n", errmax);
        ++(*fail);
    } else
        printf("SUCCESS (max relative error = %g)\n", errmax);
}

// For testing the Dawson and error functions for the special case of an
// infinite argument

void iTest(int* fail, const char* fctName, _cerf_cmplx (*F)(_cerf_cmplx), double (*FRE)(double))
{
    printf("############# %s(inf) tests ###########\n", fctName);
    double errmax = 0;
    double re_err = relerr(FRE(Inf), creal(F(C(Inf, 0.))));
    if (re_err > errmax)
        errmax = re_err;
    re_err = relerr(FRE(-Inf), creal(F(C(-Inf, 0.))));
    if (re_err > errmax)
        errmax = re_err;
    re_err = relerr(FRE(NaN), creal(F(C(NaN, 0.))));
    if (re_err > errmax)
        errmax = re_err;
    if (errmax > errBound) {
        printf("FAILURE -- relative error %g too large!\n", errmax);
        ++(*fail);
    } else
        printf("SUCCESS (max relative error = %g)\n", errmax);
}

/******************************************************************************/
/*  Test routines for specific library functions                              */
/******************************************************************************/

/******************************************************************************/
void test_erfi(int* fail)
{
    result_t result = {0, 0};

    // since erfi just calls through to erf, just one test should
    // be sufficient to make sure I didn't screw up the signs or something
    ZTEST(
        result, 1e-15, cerfi(C(1.234, 0.5678)),
        C(1.081032284405373149432716643834106923212, 1.926775520840916645838949402886591180834));

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
void test_erfcx(int* fail)
{
    result_t result = {0, 0};
    // since erfcx just calls through to w, just one test should
    // be sufficient to make sure I didn't screw up the signs or something

    // erfcx(z[i]), computed with Maple
    ZTEST(
        result, 1e-13, cerfcx(C(1.234, 0.5678)),
        C(0.3382187479799972294747793561190487832579, -0.1116077470811648467464927471872945833154));

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
void test_w_of_z(int* fail)
{
    result_t result = {0, 0};
    /* w(z), computed with WolframAlpha
                     ... note that WolframAlpha is problematic
                     some of the above inputs, so I had to
                     use the continued-fraction expansion
                     in WolframAlpha in some cases, or switch
                     to Maple */
    ZTEST(
        result, 1e-13, w_of_z(C(624.2, -0.26123)),
        C(-3.78270245518980507452677445620103199303131110e-7,
          0.000903861276433172057331093754199933411710053155));
    ZTEST(
        result, 1e-13, w_of_z(C(-0.4, 3.)),
        C(0.1764906227004816847297495349730234591778719532788,
          -0.02146550539468457616788719893991501311573031095617));
    ZTEST(
        result, 1e-13, w_of_z(C(0.6, 2.)),
        C(0.2410250715772692146133539023007113781272362309451,
          0.06087579663428089745895459735240964093522265589350));
    ZTEST(
        result, 1e-13, w_of_z(C(-1., 1.)),
        C(0.30474420525691259245713884106959496013413834051768,
          -0.20821893820283162728743734725471561394145872072738));
    ZTEST(
        result, 1e-13, w_of_z(C(-1., -9.)),
        C(7.317131068972378096865595229600561710140617977e34,
          8.321873499714402777186848353320412813066170427e34));
    ZTEST(
        result, 1e-13, w_of_z(C(-1., 9.)),
        C(0.0615698507236323685519612934241429530190806818395,
          -0.00676005783716575013073036218018565206070072304635));
    ZTEST(
        result, 1e-13, w_of_z(C(-0.0000000234545, 1.1234)),
        C(0.3960793007699874918961319170187598400134746631,
          -5.593152259116644920546186222529802777409274656e-9));
    ZTEST(
        result, 1e-13, w_of_z(C(-3., 5.1)),
        C(0.08217199226739447943295069917990417630675021771804,
          -0.04701291087643609891018366143118110965272615832184));
    ZTEST(
        result, 1e-13, w_of_z(C(-53, 30.1)),
        C(0.00457246000350281640952328010227885008541748668738,
          -0.00804900791411691821818731763401840373998654987934));
    ZTEST(
        result, 1e-13, w_of_z(C(0.0, 0.12345)),
        C(0.8746342859608052666092782112565360755791467973338452, 0.));
    ZTEST(
        result, 1e-13, w_of_z(C(11, 1)),
        C(0.00468190164965444174367477874864366058339647648741,
          0.0510735563901306197993676329845149741675029197050));
    ZTEST(
        result, 1e-13, w_of_z(C(-22, -2)),
        C(-0.0023193175200187620902125853834909543869428763219,
          -0.025460054739731556004902057663500272721780776336));
    ZTEST(
        result, 1e-13, w_of_z(C(9, -28)),
        C(9.11463368405637174660562096516414499772662584e304,
          3.97101807145263333769664875189354358563218932e305));
    ZTEST(
        result, 1e-13, w_of_z(C(21, -33)),
        C(-4.4927207857715598976165541011143706155432296e281,
          -2.8019591213423077494444700357168707775769028e281));
    ZTEST(
        result, 1e-13, w_of_z(C(1e5, 1e5)),
        C(2.820947917809305132678577516325951485807107151e-6,
          2.820947917668257736791638444590253942253354058e-6));
    ZTEST(
        result, 1e-13, w_of_z(C(1e14, 1e14)),
        C(2.82094791773878143474039725787438662716372268e-15,
          2.82094791773878143474039725773333923127678361e-15));
    ZTEST(
        result, 1e-13, w_of_z(C(-3001, -1000)),
        C(-0.0000563851289696244350147899376081488003110150498,
          -0.000169211755126812174631861529808288295454992688));
    ZTEST(
        result, 1e-13, w_of_z(C(1e160, -1e159)),
        C(-5.586035480670854326218608431294778077663867e-162,
          5.586035480670854326218608431294778077663867e-161));
    ZTEST(
        result, 1e-13, w_of_z(C(-6.01, 0.01)),
        C(0.00016318325137140451888255634399123461580248456,
          -0.095232456573009287370728788146686162555021209999));
    ZTEST(
        result, 1e-13, w_of_z(C(-0.7, -0.7)),
        C(0.69504753678406939989115375989939096800793577783885,
          -1.8916411171103639136680830887017670616339912024317));
    ZTEST(
        result, 1e-13, w_of_z(C(2.611780000000000e+01, 4.540909610972489e+03)),
        C(0.0001242418269653279656612334210746733213167234822,
          7.145975826320186888508563111992099992116786763e-7));
    ZTEST(
        result, 1e-13, w_of_z(C(0.8e7, 0.3e7)),
        C(2.318587329648353318615800865959225429377529825e-8,
          6.182899545728857485721417893323317843200933380e-8));
    ZTEST(
        result, 1e-13, w_of_z(C(-20, -19.8081)),
        C(-0.0133426877243506022053521927604277115767311800303,
          -0.0148087097143220769493341484176979826888871576145));
    ZTEST(
        result, 1e-13, w_of_z(C(1e-16, -1.1e-16)),
        C(1.00000000000000012412170838050638522857747934,
          1.12837916709551279389615890312156495593616433e-16));
    ZTEST(
        result, 1e-13, w_of_z(C(2.3e-8, 1.3e-8)),
        C(0.9999999853310704677583504063775310832036830015,
          2.595272024519678881897196435157270184030360773e-8));
    ZTEST(
        result, 1e-13, w_of_z(C(6.3, -1e-13)),
        C(-1.4731421795638279504242963027196663601154624e-15,
          0.090727659684127365236479098488823462473074709));
    ZTEST(
        result, 1e-13, w_of_z(C(6.3, 1e-20)),
        C(5.79246077884410284575834156425396800754409308e-18,
          0.0907276596841273652364790985059772809093822374));
    ZTEST(
        result, 1e-13, w_of_z(C(1e-20, 6.3)),
        C(0.0884658993528521953466533278764830881245144368,
          1.37088352495749125283269718778582613192166760e-22));
    ZTEST(
        result, 1e-13, w_of_z(C(1e-20, 16.3)),
        C(0.0345480845419190424370085249304184266813447878,
          2.11161102895179044968099038990446187626075258e-23));
    ZTEST(
        result, 1e-13, w_of_z(C(9, 1e-300)),
        C(6.63967719958073440070225527042829242391918213e-36,
          0.0630820900592582863713653132559743161572639353));
    ZTEST(
        result, 1e-13, w_of_z(C(6.01, 0.11)),
        C(0.00179435233208702644891092397579091030658500743634,
          0.0951983814805270647939647438459699953990788064762));
    ZTEST(
        result, 1e-13, w_of_z(C(8.01, 1.01e-10)),
        C(9.09760377102097999924241322094863528771095448e-13,
          0.0709979210725138550986782242355007611074966717));
    ZTEST(
        result, 1e-13, w_of_z(C(28.01, 1e-300)),
        C(7.2049510279742166460047102593255688682910274423e-304,
          0.0201552956479526953866611812593266285000876784321));
    ZTEST(
        result, 1e-13, w_of_z(C(10.01, 1e-200)),
        C(3.04543604652250734193622967873276113872279682e-44,
          0.0566481651760675042930042117726713294607499165));
    ZTEST(
        result, 1e-13, w_of_z(C(10.01, -1e-200)),
        C(3.04543604652250734193622967873276113872279682e-44,
          0.0566481651760675042930042117726713294607499165));
    ZTEST(
        result, 1e-13, w_of_z(C(10.01, 0.99e-10)),
        C(0.5659928732065273429286988428080855057102069081e-12,
          0.056648165176067504292998527162143030538756683302));
    ZTEST(
        result, 1e-13, w_of_z(C(10.01, -0.99e-10)),
        C(-0.56599287320652734292869884280802459698927645e-12,
          0.0566481651760675042929985271621430305387566833029));
    ZTEST(
        result, 1e-13, w_of_z(C(1e-20, 7.01)),
        C(0.0796884251721652215687859778119964009569455462,
          1.11474461817561675017794941973556302717225126e-22));
    ZTEST(
        result, 1e-13, w_of_z(C(-1, 7.01)),
        C(0.07817195821247357458545539935996687005781943386550,
          -0.01093913670103576690766705513142246633056714279654));
    ZTEST(
        result, 1e-13, w_of_z(C(5.99, 7.01)),
        C(0.04670032980990449912809326141164730850466208439937,
          0.03944038961933534137558064191650437353429669886545));
    ZTEST(
        result, 1e-13, w_of_z(C(1, 0)),
        C(0.36787944117144232159552377016146086744581113103176,
          0.60715770584139372911503823580074492116122092866515));
    ZTEST(
        result, 1e-13, w_of_z(C(55, 0)),
        C(0, 0.010259688805536830986089913987516716056946786526145));
    ZTEST(
        result, 1e-13, w_of_z(C(-0.1, 0)),
        C(0.99004983374916805357390597718003655777207908125383,
          -0.11208866436449538036721343053869621153527769495574));
    ZTEST(
        result, 1e-13, w_of_z(C(1e-20, 0)),
        C(0.99999999999999999999999999999999999999990000,
          1.12837916709551257389615890312154517168802603e-20));
    ZTEST(
        result, 1e-13, w_of_z(C(0, 5e-14)),
        C(0.999999999999943581041645226871305192054749891144158, 0));
    ZTEST(
        result, 1e-13, w_of_z(C(0, 51)),
        C(0.0110604154853277201542582159216317923453996211744250, 0));
    ZTEST(result, 1e-13, w_of_z(C(Inf, 0)), C(0, 0));
    ZTEST(result, 1e-13, w_of_z(C(-Inf, 0)), C(0, 0));
    ZTEST(result, 1e-13, w_of_z(C(0, Inf)), C(0, 0));
    ZTEST(result, 1e-13, w_of_z(C(0, -Inf)), C(Inf, 0));
    ZTEST(result, 1e-13, w_of_z(C(Inf, Inf)), C(0, 0));
    ZTEST(result, 1e-13, w_of_z(C(Inf, -Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, w_of_z(C(NaN, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, w_of_z(C(NaN, 0)), C(NaN, NaN));
    ZTEST(result, 1e-13, w_of_z(C(0, NaN)), C(NaN, 0));
    ZTEST(result, 1e-13, w_of_z(C(NaN, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, w_of_z(C(Inf, NaN)), C(NaN, NaN));

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
void test_erf(int* fail)
{
    result_t result = {0, 0};
    // erf(z[i]), evaluated with Maple
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

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
void test_erfc(int* fail)
{
    result_t result = {0, 0};
    // erfc(z[i]), evaluated with Maple
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

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
void test_dawson(int* fail)
{
    result_t result = {0, 0};
    // dawson(z[i]), evaluated with Maple
    ZTEST(
        result, 1e-13, cdawson(C(2, 1)),
        C(0.1635394094345355614904345232875688576839, -0.1531245755371229803585918112683241066853));
    ZTEST(
        result, 1e-13, cdawson(C(-2, 1)),
        C(-0.1635394094345355614904345232875688576839,
          -0.1531245755371229803585918112683241066853));
    ZTEST(
        result, 1e-13, cdawson(C(2, -1)),
        C(0.1635394094345355614904345232875688576839, 0.1531245755371229803585918112683241066853));
    ZTEST(
        result, 1e-13, cdawson(C(-2, -1)),
        C(-0.1635394094345355614904345232875688576839, 0.1531245755371229803585918112683241066853));
    ZTEST(
        result, 1e-13, cdawson(C(-28, 9)),
        C(-0.01619082256681596362895875232699626384420,
          -0.005210224203359059109181555401330902819419));
    ZTEST(
        result, 1e-13, cdawson(C(33, -21)),
        C(0.01078377080978103125464543240346760257008,
          0.006866888783433775382193630944275682670599));
    ZTEST(
        result, 1e-13, cdawson(C(1e3, 1e3)),
        C(-0.5808616819196736225612296471081337245459, 0.6688593905505562263387760667171706325749));
    ZTEST(result, 1e-13, cdawson(C(-1000, -3001)), C(Inf, -Inf));
    ZTEST(
        result, 1e-13, cdawson(C(1e-8, 5.1e-3)),
        C(0.1000052020902036118082966385855563526705e-7,
          0.005100088434920073153418834680320146441685));
    ZTEST(
        result, 1e-13, cdawson(C(4.95e-3, -4.9e-3)),
        C(0.004950156837581592745389973960217444687524,
          -0.004899838305155226382584756154100963570500));
    ZTEST(
        result, 1e-13, cdawson(C(5.1e-3, 5.1e-3)),
        C(0.005100176864319675957314822982399286703798,
          0.005099823128319785355949825238269336481254));
    ZTEST(
        result, 1e-13, cdawson(C(0.5, 4.9e-3)),
        C(0.4244534840871830045021143490355372016428,
          0.002820278933186814021399602648373095266538));
    ZTEST(
        result, 1e-13, cdawson(C(-0.5e1, 4.9e-4)),
        C(-0.1021340733271046543881236523269967674156,
          -0.00001045696456072005761498961861088944159916));
    ZTEST(
        result, 1e-13, cdawson(C(-0.5e2, -4.9e-5)),
        C(-0.01000200120119206748855061636187197886859,
          0.9805885888237419500266621041508714123763e-8));
    ZTEST(
        result, 1e-13, cdawson(C(0.5e3, 4.9e-6)),
        C(0.001000002000012000023960527532953151819595,
          -0.9800058800588007290937355024646722133204e-11));
    ZTEST(
        result, 1e-13, cdawson(C(0.5, 5.1e-3)),
        C(0.4244549085628511778373438768121222815752,
          0.002935393851311701428647152230552122898291));
    ZTEST(
        result, 1e-13, cdawson(C(-0.5e1, 5.1e-4)),
        C(-0.1021340732357117208743299813648493928105,
          -0.00001088377943049851799938998805451564893540));
    ZTEST(
        result, 1e-13, cdawson(C(-0.5e2, -5.1e-5)),
        C(-0.01000200120119126652710792390331206563616,
          0.1020612612857282306892368985525393707486e-7));
    ZTEST(
        result, 1e-13, cdawson(C(1e-6, 2e-6)),
        C(0.1000000000007333333333344266666666664457e-5,
          0.2000000000001333333333323199999999978819e-5));
    ZTEST(result, 1e-13, cdawson(C(2e-6, 0)), C(0.1999999999994666666666675199999999990248e-5, 0));
    ZTEST(result, 1e-13, cdawson(C(2, 0)), C(0.3013403889237919660346644392864226952119, 0));
    ZTEST(result, 1e-13, cdawson(C(20, 0)), C(0.02503136792640367194699495234782353186858, 0));
    ZTEST(result, 1e-13, cdawson(C(200, 0)), C(0.002500031251171948248596912483183760683918, 0));
    ZTEST(result, 1e-13, cdawson(C(0, 4.9e-3)), C(0, 0.004900078433419939164774792850907128053308));
    ZTEST(
        result, 1e-13, cdawson(C(0, -5.1e-3)), C(0, -0.005100088434920074173454208832365950009419));
    ZTEST(result, 1e-13, cdawson(C(0, 2e-6)), C(0, 0.2000000000005333333333341866666666676419e-5));
    ZTEST(result, 1e-13, cdawson(C(0, -2)), C(0, -48.16001211429122974789822893525016528191));
    ZTEST(result, 1e-13, cdawson(C(0, 20)), C(0, 0.4627407029504443513654142715903005954668e174));
    ZTEST(result, 1e-13, cdawson(C(0, -200)), C(0, -Inf));
    ZTEST(result, 1e-13, cdawson(C(Inf, 0)), C(0, 0));
    ZTEST(result, 1e-13, cdawson(C(-Inf, 0)), C(-0, 0));
    ZTEST(result, 1e-13, cdawson(C(0, Inf)), C(0, Inf));
    ZTEST(result, 1e-13, cdawson(C(0, -Inf)), C(0, -Inf));
    ZTEST(result, 1e-13, cdawson(C(Inf, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cdawson(C(Inf, -Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cdawson(C(NaN, NaN)), C(NaN, NaN));
    ZTEST(result, 1e-13, cdawson(C(NaN, 0)), C(NaN, 0));
    ZTEST(result, 1e-13, cdawson(C(0, NaN)), C(0, NaN));
    ZTEST(result, 1e-13, cdawson(C(NaN, Inf)), C(NaN, NaN));
    ZTEST(result, 1e-13, cdawson(C(Inf, NaN)), C(NaN, NaN));
    ZTEST(
        result, 1e-13, cdawson(C(39, 6.4e-5)),
        C(0.01282473148489433743567240624939698290584,
          -0.2105957276516618621447832572909153498104e-7));
    ZTEST(
        result, 1e-13, cdawson(C(41, 6.09e-5)),
        C(0.01219875253423634378984109995893708152885,
          -0.1813040560401824664088425926165834355953e-7));
    ZTEST(
        result, 1e-13, cdawson(C(4.9e7, 5e-11)),
        C(0.1020408163265306334945473399689037886997e-7,
          -0.1041232819658476285651490827866174985330e-25));
    ZTEST(
        result, 1e-13, cdawson(C(5.1e7, 4.8e-11)),
        C(0.9803921568627452865036825956835185367356e-8,
          -0.9227220299884665067601095648451913375754e-26));
    ZTEST(
        result, 1e-13, cdawson(C(1e9, 2.4e-12)),
        C(0.5000000000000000002500000000000000003750e-9,
          -0.1200000000000000001800000188712838420241e-29));
    ZTEST(
        result, 1e-13, cdawson(C(1e11, 2.4e-14)),
        C(5.00000000000000000000025000000000000000000003e-12,
          -1.20000000000000000000018000000000000000000004e-36));
    ZTEST(
        result, 1e-13, cdawson(C(1e13, 2.4e-16)),
        C(5.00000000000000000000000002500000000000000000e-14,
          -1.20000000000000000000000001800000000000000000e-42));
    ZTEST(result, 1e-13, cdawson(C(1e300, 2.4e-303)), C(5e-301, 0));

    printf("%i/%i tests failed", result.failed, result.total);
    // return result.failed ? 1 : 0;
    *fail += result.failed;
}

/******************************************************************************/
/*  Main: test sequence                                                       */
/******************************************************************************/

int main(void)
{
    int fail = 0;

    test_w_of_z(&fail);

    test_erf(&fail);
    xTest(&fail, "erf", cerf, erf, 1e-20);
    iTest(&fail, "erf", cerf, erf);

    test_erfi(&fail);
    xTest(&fail, "erfi", cerf, erf, 0);
    iTest(&fail, "erfi", cerf, erf);

    test_erfc(&fail);
    xTest(&fail, "erfc", cerf, erf, 1e-20);
    iTest(&fail, "erfc", cerf, erf);

    test_erfcx(&fail);
    xTest(&fail, "erfcx", cerf, erf, 0);
    iTest(&fail, "erfcx", cerf, erf);

    test_dawson(&fail);
    xTest(&fail, "dawson", cerf, erf, 1e-20);
    iTest(&fail, "dawson", cerf, erf);

    printf("#####################################\n");
    if (fail) {
        printf("IN TOTAL, FAILURE IN %i TESTS\n", fail);
        return 1;
    } else {
        printf("OVERALL SUCCESS\n");
        return 0;
    }
}
