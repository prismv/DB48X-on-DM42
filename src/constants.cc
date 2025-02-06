// ****************************************************************************
//  constants.cc                                                  DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Constant values loaded from a constants file
//
//
//
//
//
//
//
//
// ****************************************************************************
//   (C) 2024 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the terms outlined in LICENSE.txt
// ****************************************************************************
//   This file is part of DB48X.
//
//   DB48X is free software: you can redistribute it and/or modify
//   it under the terms outlined in the LICENSE.txt file
//
//   DB48X is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// ****************************************************************************

#include "constants.h"

#include "algebraic.h"
#include "arithmetic.h"
#include "compare.h"
#include "expression.h"
#include "file.h"
#include "files.h"
#include "functions.h"
#include "grob.h"
#include "parser.h"
#include "renderer.h"
#include "settings.h"
#include "unit.h"
#include "user_interface.h"
#include "utf8.h"

RECORDER(constants,         16, "Constant objects");
RECORDER(constants_error,   16, "Error on constant objects");


// ============================================================================
//
//   Parsing the constant from teh constant file
//
// ============================================================================

PARSE_BODY(constant)
// ----------------------------------------------------------------------------
//    Skip, the actual parsing is done in the symbol parser
// ----------------------------------------------------------------------------
{
    return do_parsing(constants, p);
}


SIZE_BODY(constant)
// ----------------------------------------------------------------------------
//   Compute the size
// ----------------------------------------------------------------------------
{
    object_p p = object_p(payload(o));
    p += leb128size(p);
    return byte_p(p) - byte_p(o);
}


RENDER_BODY(constant)
// ----------------------------------------------------------------------------
//   Render the constant into the given constant buffer
// ----------------------------------------------------------------------------
{
    return do_rendering(constants, o, r);
}


GRAPH_BODY(constant)
// ----------------------------------------------------------------------------
//   Do not italicize constants, but render as bold
// ----------------------------------------------------------------------------
{
    using pixsize = grob::pixsize;

    grob_g sym = object::do_graph(o, g);
    if (!sym)
        return nullptr;

    pixsize sw    = sym->width();
    pixsize sh    = sym->height();
    pixsize rw    = sw + 1;
    pixsize rh    = sh;
    grob_g result = g.grob(rw, rh);
    if (!result)
        return nullptr;

    grob::surface ss = sym->pixels();
    grob::surface rs = result->pixels();

    rs.fill(0, 0, rw, rh, g.background);
    rs.copy(ss, 0, 0);
    blitter::blit<blitter::DRAW>(rs, ss,
                                 rect(1, 0, sw, sh-1), point(),
                                 blitter::blitop_and, pattern::black);

    return result;
}


EVAL_BODY(constant)
// ----------------------------------------------------------------------------
//   Check if we need to convert to numeric
// ----------------------------------------------------------------------------
{
    // Check if we should preserve the constant as is
    if (!Settings.NumericalConstants() && !Settings.NumericalResults())
        return rt.push(o) ? OK : ERROR;
    algebraic_g value = o->value();
    return rt.push(+value) ? OK : ERROR;
}


HELP_BODY(constant)
// ----------------------------------------------------------------------------
//   Help topic for constants
// ----------------------------------------------------------------------------
{
    return o->do_instance_help(constant::constants);
}


MENU_BODY(constant_menu)
// ----------------------------------------------------------------------------
//   Build a constants menu
// ----------------------------------------------------------------------------
{
    return o->do_submenu(constant::constants, mi);
}


HELP_BODY(constant_menu)
// ----------------------------------------------------------------------------
//   Show the help for the given constant
// ----------------------------------------------------------------------------
{
    return o->do_menu_help(constant::constants, o);
}


MENU_BODY(ConstantsMenu)
// ----------------------------------------------------------------------------
//   The constants menu is dynamically populated
// ----------------------------------------------------------------------------
{
    return constant::do_collection_menu(constant::constants, mi);
}


utf8 constant_menu::name(id type, size_t &len)
// ----------------------------------------------------------------------------
//   Return the name for a menu entry
// ----------------------------------------------------------------------------
{
    return do_name(constant::constants, type, len);
}


COMMAND_BODY(ConstantName)
// ----------------------------------------------------------------------------
//   Put the name of a constant on the stack
// ----------------------------------------------------------------------------
{
    int key = ui.evaluating;
    if (object_p cstobj = constant::do_key(constant::constants, key))
        if (constant_p cst = cstobj->as<constant>())
            if (rt.push(cst))
                return OK;
    if (!rt.error())
        rt.type_error();
    return ERROR;
}


INSERT_BODY(ConstantName)
// ----------------------------------------------------------------------------
//   Put the name of a constant in the editor
// ----------------------------------------------------------------------------
{
    int key = ui.evaluating;
    return ui.insert_softkey(key, " Ⓒ", " ", false);
}


HELP_BODY(ConstantName)
// ----------------------------------------------------------------------------
//   Put the help for a given constant name
// ----------------------------------------------------------------------------
{
    int key = ui.evaluating;
    if (object_p cstobj = constant::do_key(constant::constants, key))
        if (constant_p cst = cstobj->as<constant>())
            return cst->help();
    return utf8("Constants");
}


COMMAND_BODY(ConstantValue)
// ----------------------------------------------------------------------------
//   Put the value of a constant on the stack
// ----------------------------------------------------------------------------
{
    int key = ui.evaluating;
    if (object_p cstobj = constant::do_key(constant::constants, key))
        if (constant_p cst = cstobj->as<constant>())
            if (algebraic_p value = cst->numerical_value())
                if (rt.push(value))
                    return OK;
    if (!rt.error())
        rt.type_error();
    return ERROR;
}


INSERT_BODY(ConstantValue)
// ----------------------------------------------------------------------------
//   Insert the value of a constant
// ----------------------------------------------------------------------------
{
    int key = ui.evaluating;
    if (object_p cstobj = constant::do_key(constant::constants, key))
        if (constant_p cst = cstobj->as<constant>())
            if (object_p value = cst->value())
                return ui.insert_object(value, " ", " ");
    return ERROR;
}


HELP_BODY(ConstantValue)
// ----------------------------------------------------------------------------
//   Put the help for a given constant name
// ----------------------------------------------------------------------------
{
    return ConstantName::do_help(nullptr);
}


COMMAND_BODY(Const)
// ----------------------------------------------------------------------------
//   Evaluate a library constant
// ----------------------------------------------------------------------------
{
    return constant::lookup_command(constant::constants, true);
}


COMMAND_BODY(Constants)
// ----------------------------------------------------------------------------
//   Select the mathematics constants menu
// ----------------------------------------------------------------------------
{
    if (object_p menu = constant::lookup_menu(constant::constants,
                                              "Mathematics"))
        return menu->evaluate();
    rt.invalid_constant_error();
    return ERROR;
}



// ============================================================================
//
//   Constant definitions
//   *: Algefrification & Conformity to CODATA2022
// ============================================================================

static const cstring basic_constants[] =
// ----------------------------------------------------------------------------
//   List of basic constants
// ----------------------------------------------------------------------------
//   clang-format off
{
    // ------------------------------------------------------------------------
    // MATH CONSTANTS MENU
    // ------------------------------------------------------------------------
    "Mathematics",   nullptr,

    "π",        "3.14159",              // Evaluated specially (decimal-pi.h)
    "e",        "2.71828",              // Evaluated specially (decimal-e.h)
    "ⅈ",        "0+ⅈ1",                 // Imaginary unit
    "∞",        "9.99999E999999",       // A small version of infinity
    "?",        "Undefined",            // Undefined result

    "ⅉ",        "0+ⅈ1",                 // Imaginary unit
    "rad",      "1_r",                  // One radian
    "twoπ",     "'2*Ⓒπ'_r",             // Two pi radian
    "angl",     "180_°",                // Half turn


    // ------------------------------------------------------------------------
    //   Chemistry Constants Values & Uncertainties
    // ------------------------------------------------------------------------

    "Chemistry",     nullptr,

//NA constant; *Avogadro's number - Exact definition
    "NA",       "6.02214076E23_mol⁻¹",
    "UsNA",     "0_mol⁻¹",
    "UrNA",     "0",
//K constant; *Boltzmann - Exact definition
    "k",        "1.380649E-23_J/K",
    "Usk",      "0_J/K",
    "Urk",      "0",
//Vm constant; *Molar volume - Calculation convention
    "Vm",       "'CONVERT(ⒸR*ⒸStdT/ⒸStdP;1_m^3/mol)'",
    "UsVm",     "0_m^3/mol",
    "UrVm",     "0",
//R constant; *Universal gas constant - Exact calculation
    "R",        "'CONVERT(ⒸNA*Ⓒk;1_J/(mol*K))'",
    "UsR",      "0_J/(mol*K)",
    "UrR",      "0",
//StdT constant; *Standard temperature - Definition convention
    "StdT",     "273.15_K",
    "UsStdT",   "0_K",
    "UrStdT",   "0",
//StdP constant; *Standard pressure - Definition convention
    "StdP",     "101.325_kPa",
    "UsStdP",   "0_kPa",
    "UrStdP",   "0",
//σ constant; *Stefan-Boltzmann - Exact calculation
    "σ",        "'CONVERT(Ⓒπ^2/60*Ⓒk^4/(Ⓒℏ^3*Ⓒc^2);1_W/(m^2*K^4))'",
    "Usσ",      "0_W/(m^2*K^4)",
    "Urσ",      "0",
//Mu constant; *Molar Mass Constant - Calculation from measurement
    "Mu",      "'→NUM(ROUND(CONVERT(ⒸNA*Ⓒu;1_kg/mol);XPON(UVAL(ⒸUrMu*ⒸNA*Ⓒu))-XPON(UVAL(ⒸNA*Ⓒu))-2))'",
    "UsMu",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrMu*ⒸMu);-2);1_kg/mol))'",//=>UsMu='→NUM(CONVERT(ROUND(UBASE(UrMu*Mu);-2);1_kg/mol))'=3.1E-13 kg/mol
    "UrMu",    "'ⒸUru'",                                   // UrMu=3.1E-10
  // UrMu=3.1E-10 NA=6.02214076E23_mol⁻¹ u=1.66053906892E-27_kg => Mu=1.00000000105E-3 kg/mol
  //'→NUM(ROUND(CONVERT(NA*u;1_kg/mol);XPON(UVAL(UrMu*NA*u))-XPON(UVAL(NA*u))-2))'
  //'→NUM(ROUND(CONVERT(NA*u;1_kg/mol);XPON(UrMu*NA*u)-XPON(NA*u)-2))' Doesn't work: UVAL is required
//MC12 constant; *C12 Molar Mass - Calculation from measurement
    "MC12",      "'→NUM(ROUND(CONVERT(12*ⒸMu;1_kg/mol);XPON(UVAL(ⒸUrMC12*12*ⒸMu))-XPON(UVAL(12*ⒸMu))-2))'",
    "UsMC12",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrMC12*ⒸMC12);-2);1_kg/mol))'",//=>UsMC12='→NUM(CONVERT(ROUND(UBASE(UrMC12*MC12);-2);1_kg/mol))'=3.7E-12 kg/mol
    "UrMC12",    "'ⒸUrMu'",                                   // UrMu=3.1E-10
  // UrMC12=3.1E-10 NA=6.02214076E23_mol⁻¹ u=1.66053906892E-27_kg => MC12=12.0000000126E-3 kg/mol
  //'→NUM(ROUND(CONVERT(12*Mu;1_kg/mol);XPON(UVAL(UrMC12*12*Mu))-XPON(UVAL(12*Mu))-2))'
//Are constant; *Electron relative atomic mass - Measurement
    "Are",       "5.485799090441E-4",
    "UsAre",     "0.000000000097E-4",
    "UrAre",     "'→NUM(ROUND(UBASE(ⒸUsAre/ⒸAre);-2))'",
  // Are=5.485799090441E-4 UsAre=0.000000000097E-4 '→NUM(ROUND(UBASE(UsAre/Are);-2))' => UrAre=1.8E-11
//n0 constant; *Loschmidt constant - Exact calculation
    "n0",        "'CONVERT(ⒸNA/ⒸVm;1_m^-3)'",
    "Un0",      "0_m^-3",
    "Ur0",      "0",
  // n0=2.68678 01118E25_(m↑3)⁻¹
//SoR constant; *Sakur-Tetrode constant - Calculation from measurement
    "SoR",      "'→NUM(ROUND((5/2+LN(UBASE(Ⓒu*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP));XPON(ⒸUrSoR*(5/2+LN(UBASE(Ⓒu*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)))-XPON((5/2+LN(UBASE(Ⓒu*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)))-2))'",
    "UsSoR",    "'ROUND(UBASE(ⒸUrSoR*ABS(ⒸSoR));-2)'",//=>UsSoR='ROUND(UBASE(UrSoR*ABS(SoR));-2)'=4.7E-10
    "UrSoR",    "4.0E-10",        //UrSoR=4.0E-10
  // UrSoR=4.0E-10 u=1.66053906892E-27_kg => SoR=-1.16487052149 <=>
  //'→NUM(ROUND((5/2+LN(UBASE(u*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP));XPON(UrSoR*(5/2+LN(UBASE(u*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)))-XPON((5/2+LN(UBASE(u*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)))-2))'

    // ------------------------------------------------------------------------
    //   Physics Constants Values & Uncertainties
    // ------------------------------------------------------------------------

    "Physics",     nullptr,

//ⅉ constant; *Imaginary unit in physics - Definition convention
    "ⅉ",         "0+ⅈ1",
//c constant; *Speed of light - Exact definition
    "c",        "299792458_m/s",
    "Usc",      "0_m/s",
    "Urc",      "0",
//ε0 constant; *Vaccuum permittivity - Calculation from measurement
    "ε0",       "'→NUM(ROUND(CONVERT(1/(Ⓒμ0*Ⓒc^2);1_F/m);XPON(UVAL(ⒸUrε0/(Ⓒμ0*Ⓒc^2)))-XPON(UVAL(1/(Ⓒμ0*Ⓒc^2)))-2))'",
    "Usε0",     "'→NUM(ROUND(UBASE(ⒸUrε0*Ⓒε0);-2))'",//=>Usε0='→NUM(CONVERT(ROUND(UBASE(Urα*ε0);-2);1_F/m))'=1.4E-21 F/m
    "Urε0",     "'ⒸUrμ0'",
  // α=0.00729735256434 Urε0=1.6E-10 μ0=1.25663706127E-6 H/m => ε0=8.8541878188E-12_F/m <=>
  //'→NUM(ROUND(CONVERT(1/(μ0*Ⓒc^2);1_F/m);XPON(UVAL(Urε0/(μ0*Ⓒc^2)))-XPON(UVAL(1/(μ0*Ⓒc^2)))-2))'
//μ0 constant; *Vaccuum permeability - Calculation from measurement
    "μ0",       "'→NUM(ROUND(CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m);XPON(UVAL(ⒸUrμ0*4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc)))-XPON(UVAL(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc)))-2))'",
    "Usμ0",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrμ0*Ⓒμ0);-2);1_H/M))'",//=>Usμ0='→NUM(CONVERT(ROUND(UBASE(Urα*μ0);-2);1_H/m))'=2.0E-16 H/m
    "Urμ0",     "'ⒸUrα'",
  // α=0.00729735256434 Urα=1.6E-10 Urμ0=1.6E-10 => μ0=1.25663706127E-6 H/m <=>
  //'→NUM(ROUND(CONVERT(4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m);XPON(UVAL(Urμ0*4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc)))-XPON(UVAL(4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc)))-2))'
//g constant; *Acceleration of Earth gravity - Definition convention
    "g",        "9.80665_m/s²",
    "Usg",      "0_m/s²",
    "Urg",      "0",
//G constant; *Gravitation constant - Measurement
    "G",        "6.67430E-11_m^3/(s^2*kg)",
    "UsG",      "0.00015E-11_m^3/(s^2*kg)",
    "UrG",      "'→NUM(ROUND(UBASE(ⒸUsG/ⒸG);-2))'",
  // G=6.67430E-11_m^3/(s^2*kg) UsG=0.00015E-11_m^3/(s^2*kg) '→NUM(ROUND(UBASE(UsG/G);-2))' => UrG=2.2E-5
//h constant; *Planck - Exact definition
    "h",        "6.62607015E-34_J*s",
    "Ush",      "0_J*s",
    "Urh",      "0",
//ℏ constant; *Dirac - Exact definition
    "ℏ",        "'CONVERT(Ⓒh/(2*Ⓒπ);1_J*s)'",
    "Usℏ",      "0_J*s",
    "Urℏ",      "0",
  // ℏ='CONVERT(Ⓒh/(2*Ⓒπ);1_J*s)'=1.05457 18176 46156⁳⁻³⁴ J·s
//qe constant; *Electronic charge - Exact definition
    "qe",       "1.602176634E-19_C",
    "Usqe",     "0_C",
    "Urqe",     "0",
//me constant; *Electron mass - Calculation from measurement
    "me",       "'→NUM(ROUND(CONVERT(2*Ⓒh*ⒸR∞/((Ⓒα^2)*Ⓒc);1_kg);XPON(UVAL(ⒸUrme*2*Ⓒh*ⒸR∞/((Ⓒα^2)*Ⓒc)))-XPON(UVAL(2*Ⓒh*ⒸR∞/((Ⓒα^2)*Ⓒc)))-2))'",
    "Usme",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrme*Ⓒme);-2);1_kg))'",//=>Usme='→NUM(CONVERT(ROUND(UBASE(Urme*me);-2);1_kg))'=2.8E-40 kg
    "Urme",     "'ⒸUru'",  //Urme≈'2*Urα+UrR∞' or 'Urmu+UrAre' measured value 3.1E-10
  // Urme=3.1E-10 R∞=10973731.568157_m⁻¹ α=0.00729735256434 => me=9.1093837139E-31 kg <=>
  //'→NUM(ROUND(CONVERT(2*Ⓒh*R∞/((α^2)*Ⓒc);1_kg);XPON(UVAL(Urme*2*Ⓒh*R∞/((α^2)*Ⓒc)))-XPON(UVAL(2*Ⓒh*R∞/((α^2)*Ⓒc)))-2))'
  // Urme=3.1E-10 Are=5.485799090441E-4 u=1.66053906892E-27 kg => me=9.1093837139E-31 kg <=>
  //'→NUM(ROUND(CONVERT(Are*u;1_kg);XPON(UVAL(Urme*Are*u))-XPON(UVAL(Are*u))-2))'
//mp constant; *Proton mass - Measurement
    "mp",       "1.67262192595E-27_kg",
    "Usmp",     "0.00000000052E-27_kg",
    "Urmp",     "'→NUM(ROUND(UBASE(ⒸUsmp/Ⓒmp);-2))'",
  // mp=1.67262192595E-27_kg Usmp=0.00000000052E-27_kg '→NUM(ROUND(UBASE(Usmp/mp);-2))' => Urmp=3.1E-10
//mn constant; *Neutron mass - Measurement
    "mn",       "1.67492750056E-27_kg",
    "Usmn",     "0.00000000085E-27_kg",
    "Urmn",     "'→NUM(ROUND(UBASE(ⒸUsmn/Ⓒmn);-2))'",
  // mn=1.67492750056E-27_kg Usmn=0.00000000085E-27_kg '→NUM(ROUND(UBASE(Usmn/mn);-2))' => Urmn=5.1E-10
//mH constant; *Hydrogen mass - Measurement
    "mH",       "1.00782503223_u",
    "UsmH",     "0.00000000009_u",
    "UrmH",     "'→NUM(ROUND(UBASE(ⒸUsmH/ⒸmH);-2))'",
  // mH=1.00782503223_u UsmH=0.00000000009_u '→NUM(ROUND(UBASE(UsmH/mH);-2))' => UrmH=8.9E-11
//u constant; *Mass unit (u) - Calculation from measurement
    "u",        "'→NUM(ROUND(CONVERT(Ⓒme/ⒸAre;1_kg);XPON(UVAL(ⒸUru*Ⓒme/ⒸAre))-XPON(UV  rAL(Ⓒme/ⒸAre))-2))''",
    "Usu",      "'→NUM(CONVERT(ROUND(UBASE(ⒸUru*Ⓒu);-2);1_kg))'",//=>Usu='→NUM(CONVERT(ROUND(UBASE(Uru*u);-2);1_kg))'=5.1E-37 kg
    "Uru",      "3.1E-10",  //Uru≈'2*Urα+UrR∞+UrAre' measured value
  // Uru=3.1E-10 me=9.1093837139E-31 kg Are=5.485799090441E-4 => u=1.66053906892E-27 kg <=>
  //'→NUM(ROUND(CONVERT(me/Are;1_kg);XPON(UVAL(Uru*me/Are))-XPON(UVAL(me/Are))-2))'
  // Uru=3.1E-10 R∞=10973731.568157_m⁻¹ α=0.00729735256434 Are=5.485799090441E-4 => u=1.66053906892E-27 kg <=>
  //'→NUM(ROUND(CONVERT(2*Ⓒh*R∞/((α^2)*Ⓒc*Are);1_kg);XPON(UVAL(Uru*2*Ⓒh*R∞/((α^2)*Ⓒc*Are)))-XPON(UVAL(2*Ⓒh*R∞/((α^2)*Ⓒc*Are)))-2))'
//Da constant; *Mass unit (Dalton) - Calculation from measurement
    "Da",       "'Ⓒu'",
    "UsDa",     "'ⒸUsu'",
    "UrDa",     "'ⒸUru'",
//qme constant; *qme ratio - Calculation from measurement
    "qme",      "'→NUM(ROUND(CONVERT(Ⓒqe/Ⓒme;1_C/kg);XPON(UVAL(ⒸUrqme*Ⓒqe/Ⓒme))-XPON(UVAL(Ⓒqe/Ⓒme))-2))'",
    "Usqme",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrqme*Ⓒqme);-2);1_C/kg))'",//=>Usqme='→NUM(CONVERT(ROUND(UBASE(Urme*Ⓒqme);-2);1_C/kg))'=55. C/kg
    "Urqme",    "'ⒸUrme'",
  // Urme=3.1E-10 me=9.1093837139E-31_kg => qme=1.75882000838E11_C/kg <=>
  //'→NUM(ROUND(CONVERT(Ⓒqe/me;1_C/kg);XPON(UVAL(Urme*Ⓒqe/me))-XPON(UVAL(Ⓒqe/me))-2))'
//mpme constant; *mpme ratio - Measurement
    "mpme",     "1836.152673426",
    "Usmpme",   "0.000000032",
    "Urmpme",   "'→NUM(ROUND(UBASE(ⒸUsmpme/Ⓒmpme);-2))'",
  // mpme=1836.152673426 Usmpme=0.000000032 '→NUM(ROUND(UBASE(Usmpme/mpme);-2))' => Usmpme=1.7E-11
//α constant; *fine structure constant - Measurement
    "α",        "0.00729735256434",
    "Usα",      "0.00000000000114",
    "Urα",      "'→NUM(ROUND(UBASE(ⒸUsα/Ⓒα);-2))'",
  // α=0.00729735256434 Usα=0.00000000000114 '→NUM(ROUND(UBASE(Usα/α);-2))' => Urα=1.6E-10
//ø constant; *Magnetic flux quantum - Exact calculation
    "ø",        "'CONVERT(Ⓒπ*Ⓒℏ/Ⓒqe;1_Wb)'",
    "Usø",      "0_Wb",
    "Urø",      "0",
  // ø='CONVERT(Ⓒπ*Ⓒℏ/Ⓒqe;1_Wb)'=2.06783 38484 61929⁳⁻¹⁵ Wb
//F constant; *Faraday - Exact calculation
    "F",        "'CONVERT(ⒸNA*Ⓒqe;1_C/mol)'",
    "UsF",      "0_C/mol",
    "UrF",      "0",
  // F='CONVERT(ⒸNA*Ⓒqe;1_C/mol)'=96 485.33212 33100 2 C/mol
// F='CONVERT(ⒸNA*Ⓒqe;1_C/mol)'=96 485.33212 33100 2 C/mol
//R∞ constant; *Rydberg - Measurement
    "R∞",       "10973731.568157_m⁻¹",
    "UsR∞",     "0.000012_m⁻¹",
    "UrR∞",     "'→NUM(ROUND(UBASE(ⒸUsR∞/ⒸR∞);-2))'",
  // R∞=10973731.568157_m⁻¹ UsR∞=0.000012_m⁻¹ '→NUM(ROUND(UBASE(UsR∞/R∞);-2))' => UrR∞=1.1E-12
//a0 constant; *Bohr radius - Calculation from measurement
    "a0",       "'→NUM(ROUND(CONVERT(4*Ⓒπ*Ⓒε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2);1_nm);XPON(UVAL(ⒸUra0*4*Ⓒπ*Ⓒε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2)))-XPON(UVAL(4*Ⓒπ*Ⓒε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2)))-2))'",
    "Usa0",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUsα/Ⓒα*Ⓒa0);-2);1_nm))'",//=>Usa0='→NUM(CONVERT(ROUND(UBASE((Usα/α)*a0);-2);1_nm))'=8.3E-12 nm
    "Ura0",     "'ⒸUrα'",                                    // Ura0=1.6E-10
  // ε0=8.85418 78188E-12_F/m Ura0=1.6E-10 α=0.00729735256434 Usα=0.00000000000114 => a0=0.0529177210544 nm <=>
  //'→NUM(ROUND(CONVERT(4*Ⓒπ*ε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2);1_nm);XPON(UVAL(Ura0*4*Ⓒπ*ε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2)))-XPON(UVAL(4*Ⓒπ*ε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2)))-2))'
//μB constant; *Bohr magneton - Calculation from measurement
    "μB",       "'→NUM(ROUND(CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒme);1_J/T);XPON(UVAL(ⒸUrμB*Ⓒqe*Ⓒℏ/(2*Ⓒme)))-XPON(UVAL(Ⓒqe*Ⓒℏ/(2*Ⓒme)))-2))'",
    "UsμB",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrμB*ⒸμB);-2);1_J/T))'",//=>UsμB='→NUM(CONVERT(ROUND(UBASE(UrμB*μB);-2);1_J/T))'=2.9E-33 J/T
    "UrμB",     "'ⒸUrme'",                                    // UrμB=3.1E-10
  // UrμB=3.1E-10 => μB=9.2740100657E-24 J/T
  //'→NUM(ROUND(CONVERT(Ⓒqe*Ⓒℏ/(2*me);1_J/T);XPON(UVAL(UrμB*Ⓒqe*Ⓒℏ/(2*me)))-XPON(UVAL(Ⓒqe*Ⓒℏ/(2*me)))-2))'
//μN constant; *Nuclear magneton - Calculation from measurement
    "μN",       "'→NUM(ROUND(CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒmp);1_J/T);XPON(UVAL(ⒸUrμN*Ⓒqe*Ⓒℏ/(2*Ⓒmp)))-XPON(UVAL(Ⓒqe*Ⓒℏ/(2*Ⓒmp)))-2))'",
    "UsμN",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrμN*ⒸμN);-2);1_J/T))'",//=>UsμN='→NUM(CONVERT(ROUND(UBASE(UrμN*μN);-2);1_J/T))'=1.6E-36 J/T
    "UrμN",     "'ⒸUrmp'",                                    // UrμN=3.1E-10
  // UrμN=3.1E-10 => μN=5.0507837393E-27 J/T <=>
  //'→NUM(ROUND(CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒmp);1_J/T);XPON(UVAL(UrμN*Ⓒqe*Ⓒℏ/(2*Ⓒmp)))-XPON(UVAL(Ⓒqe*Ⓒℏ/(2*Ⓒmp)))-2))'
//λ0 constant; *Photon wavelength - Exact calculation
    "λ0",       "'CONVERT(Ⓒh*Ⓒc/Ⓒqe/(1_V);1_nm)'",
    "Usλ0",     "0_nm",
    "Urλ0",     "0",
  // λ0='CONVERT(Ⓒh*Ⓒc/Ⓒqe/(1_V);1_nm)'=
//f0 constant; *Photon frequency - Exact calculation1 239.84198 43320 03 nm
    "f0",       "'CONVERT(Ⓒc/Ⓒλ0;1_Hz)'",
    "Usf0",     "0_Hz",
    "Urf0",     "0",
  // f0='CONVERT(Ⓒc/Ⓒλ0;1_Hz)'=2.41798 92420 84918⁳¹⁴ Hz
//λc constant; *Electron Compton wavelength - Calculation from measurement
    "λc",       "'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒme*Ⓒc);1_nm);XPON(UVAL(ⒸUrλc*Ⓒh/(Ⓒme*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒme*Ⓒc)))-2))'",
    "Usλc",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrλc*Ⓒλc);-2);1_nm))'",//=>Usλc='→NUM(CONVERT(ROUND(UBASE(Urλc*λc);-2);1_nm))'=7.5E-13 nm
    "Urλc",     "'ⒸUrme'",                                   // Urλc=3.1E-10
  // Urλc=3.1E-10 => λc=0.00242631023538 nm <=>
  //'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒme*Ⓒc);1_nm);XPON(UVAL(Urλc*Ⓒh/(Ⓒme*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒme*Ⓒc)))-2))'
//λcp constant; *Proton Compton wavelength - Calculation from measurement
    "λcp",      "'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmp*Ⓒc);1_nm);XPON(UVAL(ⒸUrλcp*Ⓒh/(Ⓒmp*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmp*Ⓒc)))-2))'",
    "Usλcp",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrλcp*Ⓒλcp);-2);1_nm))'",//=>Usλcp='→NUM(CONVERT(ROUND(UBASE(Urλcp*λcp);-2);1_nm))'=4.1E-16 nm
    "Urλcp",    "'ⒸUrmp'",                                   // Urλcp=3.1E-10
  // Urλcp=3.1E-10 => λcp=0.00000132140985360 nm <=>
  //'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmp*Ⓒc);1_nm);XPON(UVAL(Urλcp*Ⓒh/(Ⓒmp*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmp*Ⓒc)))-2))'
//λcn constant; *Neutron Compton wavelength - Calculation from measurement
    "λcn",      "'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmn*Ⓒc);1_nm);XPON(UVAL(ⒸUrλcn*Ⓒh/(Ⓒmn*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmn*Ⓒc)))-2))'",
    "Usλcn",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrλcn*Ⓒλcn);-2);1_nm))'",//=>Usλcn='→NUM(CONVERT(ROUND(UBASE(Urλcn*λcn);-2);1_nm))'=6.7E-16 nm
    "Urλcn",    "'ⒸUrmn'",                                   // Urλcn=5.1E-10
  // Urλcn=5.1E-10 => λcn=1.31959090382E-6 nm <=>
  //'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmn*Ⓒc);1_nm);XPON(UVAL(Urλcn*Ⓒh/(Ⓒmn*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmn*Ⓒc)))-2))'
//λcμ constant; *Muon Compton wavelength - Calculation from measurement
    "λcμ",      "'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmμ*Ⓒc);1_nm);XPON(UVAL(ⒸUrλcμ*Ⓒh/(Ⓒmμ*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmμ*Ⓒc)))-2))'",
    "Usλcμ",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrλcμ*Ⓒλcμ);-2);1_nm))'",//=>Usλcμ='→NUM(CONVERT(ROUND(UBASE(Urλcμ*λcμ);-2);1_nm))'=2.6E-13 nm
    "Urλcμ",    "'ⒸUrmμ'",                                   // Urλcμ=2.2E-8
  // Urλcμ=2.2E-8 mμ=0.1134289257_u => λcμ=1.1734441110E-5 nm <=>
  //'→NUM(ROUND(CONVERT(Ⓒh/(mμ*Ⓒc);1_nm);XPON(UVAL(Urλcμ*Ⓒh/(mμ*Ⓒc)))-XPON(UVAL(Ⓒh/(mμ*Ⓒc)))-2))'
//λcτ constant; *Tau Compton wavelength - Calculation from measurement
    "λcτ",      "'→NUM(ROUND(CONVERT(Ⓒh/(Ⓒmτ*Ⓒc);1_nm);XPON(UVAL(ⒸUrλcτ*Ⓒh/(Ⓒmτ*Ⓒc)))-XPON(UVAL(Ⓒh/(Ⓒmτ*Ⓒc)))-2))'",
    "Usλcτ",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrλcτ*Ⓒλcτ);-2);1_nm))'",//=>Usλcτ='→NUM(CONVERT(ROUND(UBASE(Urλcτ*λcτ);-2);1_nm))'=4.7E-11 nm
    "Urλcτ",    "'ⒸUrmτ'",                                   // Urλcτ=6.8E-5
  // mτ=1.90754_u Usmτ=0.00013_u Urλcτ=6.8E-5 => λcτ=6.977705E-7 nm <=>
  //'→NUM(ROUND(CONVERT(Ⓒh/(mτ*Ⓒc);1_nm);XPON(UVAL(Urλcτ*Ⓒh/(mτ*Ⓒc)))-XPON(UVAL(Ⓒh/(mτ*Ⓒc)))-2))'
//c1 constant; *First radiation constant - Exact calculation
    "c1",       "'CONVERT(2*Ⓒπ*Ⓒh*Ⓒc^2;1_(W*m^2))'",
    "Usc1",     "0_(W*m^2)",
    "Urc1",     "0",
  // c1='CONVERT(2*Ⓒπ*Ⓒh*Ⓒc^2;1_(W*m^2))'=3.7417718521928E-16 W·m↑2
//c2 constant; *Second radiation constant - Exact calculation
    "c2",       "'CONVERT(Ⓒh*Ⓒc/Ⓒk;1_(m*K))'",
    "Usc2",     "0_(m*K)",
    "Urc2",     "0",
  // c2='CONVERT(Ⓒh*Ⓒc/Ⓒk;1_(m*K))'=1.4387768775039E-2 m·K
//c3 constant; *Wien's constant - Theory approximation
    "c3",       "2.897771955185172661478605448092885_mm*K",
    "Usc3",     "0_mm*K",
    "Urc3",     "0",
//c3f constant; *Wien's frequency constant - Theory approximation
    "c3f",      "0.05878925757646824946606130795309722_THz/K",
    "Usc3f",    "0_THz/K",
    "Urc3f",    "0",
//kq constant; * kq ratio - Exact calculation
    "kq",       "'CONVERT(Ⓒk/Ⓒqe;1_J/(K*C))'",
    "Uskq",     "0_J/(K*C)",
    "Urkq",     "0",
  // kq='CONVERT(Ⓒk/Ⓒqe;1_J/(K*C))'=8.6173332621452E-5 J/(K·C)
//ε0q constant; * ε0q ratio - Calculation from measurement
    "ε0q",      "'→NUM(ROUND(CONVERT(Ⓒε0/Ⓒqe;1_F/(m*C));XPON(UVAL(ⒸUrε0q*Ⓒε0/Ⓒqe))-XPON(UVAL(Ⓒε0/Ⓒqe))-2))'",
    "Usε0q",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrε0q*Ⓒε0q);-2);1_F/(m*C)))'",//=>Usε0q='→NUM(CONVERT(ROUND(UBASE(Urε0q*ε0q);-2);1_F/(m*C)))'=0.0088 F/(m·C)
    "Urε0q",    "'Urε0'",                                    // Urε0q=1.6E-10
  // Urε0q=1.6E-10 ε0=8.8541878188E-12_F/m => ε0q=55263493.6180 F/(m·C)  <=>
  //'→NUM(ROUND(CONVERT(ε0/Ⓒqe;1_F/(m*C));XPON(UVAL(Urε0q*ε0/Ⓒqe))-XPON(UVAL(ε0/Ⓒqe))-2))'
//qε0 constant; * qε0 product - Calculation from measurement
    "qε0",      "'→NUM(ROUND(CONVERT(Ⓒqe*Ⓒε0;1_F*C/m);XPON(UVAL(ⒸUrqε0*Ⓒqe*Ⓒε0))-XPON(UVAL(Ⓒqe*Ⓒε0))-2))'",
    "Usqε0",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrqε0*Ⓒqε0);-2);1_F*C/m))'",//=>Usqε0='→NUM(CONVERT(ROUND(UBASE(Urqε0*qε0);-2);1_F*C/m))'=2.3E-40 F·C/m
    "Urqε0",    "'Urε0'",                                    // Urqε0=1.6e-10
  // Urqε0=1.6e-10 ε0=8.8541878188E-12_F/m => qε0=1.41859728363E-30 F·C/m  <=>
  //'→NUM(ROUND(CONVERT(Ⓒqe*ε0;1_F*C/m);XPON(UVAL(Urqε0*Ⓒqe*ε0))-XPON(UVAL(Ⓒqe*ε0))-2))'
//ke constant; *Coulomb constant - Calculation from measurement
    "ke",     "'→NUM(ROUND(CONVERT(1/(4*Ⓒπ*Ⓒε0);1_(N*(m/C)^2));XPON(UVAL(ⒸUrke/(4*Ⓒπ*Ⓒε0)))-XPON(UVAL(1/(4*Ⓒπ*Ⓒε0)))-2))'",
    "Uske",   "'→NUM(CONVERT(ROUND(UBASE(ⒸUrke*Ⓒke);-2);1_(N*(m/C)^2)))'",//=>Uske='→NUM(CONVERT(ROUND(UBASE(Urke*ke);-2);1_(N*(m/C)^2)))'=1.4 N·m↑2/C↑2
    "Urke",   "'Urε0'",                                    // Urke=1.6e-10
  // Urke=1.6e-10 ε0=8.8541878188E-12_F/m => ke=8987551786.2 N·m↑2/C↑2 <=>
  //'→NUM(ROUND(CONVERT(1/(4*Ⓒπ*ε0);1_(N*(m/C)^2));XPON(UVAL(Urke/(4*Ⓒπ*ε0)))-XPON(UVAL(1/(4*Ⓒπ*ε0)))-2))'
//εsi constant; *Dielectric constant - Definition convention
    "εsi",      "11.9",
    "Usεsi",    "0",
    "Urεsi",    "0",
//εox constant; *SiO2 dielectric constant - Definition convention
    "εox",      "3.9",
    "Usεox",    "0",
    "Urεox",    "0",
//I0 constant; *Ref intensity - Definition convention
    "I0",       "0.000000000001_W/m^2",
    "UsI0",     "0_W/m^2",
    "UrI0",     "0",
//Z0 constant; *Vacuum characteristic impedance - Calculation from measurement
    "Z0",       "'→NUM(ROUND(CONVERT(Ⓒμ0*Ⓒc;1_Ω);XPON(UVAL(ⒸUrZ0*Ⓒμ0*Ⓒc))-XPON(UVAL(Ⓒμ0*Ⓒc))-2))'",
    "UsZ0",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrZ0*ⒸZ0);-2);1_Ω))'",//=>UsZ0='→NUM(CONVERT(ROUND(UBASE(UrZ0*Z0);-2);1_Ω))'=0.000000060 Ω
    "UrZ0",     "'ⒸUrμ0'",                                    // UrZ0=1.6e-10
  // UrZ0=1.6e-10 μ0=1.25663706127E-6 H/m => Z0=376.730313412 Ω  <=>
  //'→NUM(ROUND(CONVERT(μ0*Ⓒc;1_Ω);XPON(UVAL(UrZ0*μ0*Ⓒc))-XPON(UVAL(μ0*Ⓒc))-2))'
//mD constant; *Deuterium mass - Measurement
    "mD",       "2.01410177812_u",
    "UsmD",     "0.00000000012_u",
    "UrmD",     "'→NUM(ROUND(UBASE(ⒸUsmD/ⒸmD);-2))'",
  // mD=2.01410177812_u UsmD=0.00000000012_u '→NUM(ROUND(UBASE(UsmD/mD);-2))' => UrmD=6.0E-11
//mT constant; *Tritium mass - Measurement
    "mT",       "3.0160492779_u",
    "UsmT",     "0.0000000024_u",
    "UrmT",     "'→NUM(ROUND(UBASE(ⒸUsmT/ⒸmT);-2))'",
  // mT=3.0160492779_u UsmT=0.0000000024_u '→NUM(ROUND(UBASE(UsmT/mT);-2))' => UrmT=8.0E-10
//mHe constant; *Helium atomic mass - Measurement
    "mHe",      "4.00260325413_u",
    "UsmHe",    "0.00000000006_u",
    "UrmHe",    "'→NUM(ROUND(UBASE(ⒸUsmHe/ⒸmHe);-2))'",
  // mHe=4.00260325413_u UsmHe=0.00000000006_u '→NUM(ROUND(UBASE(UsmHe/mHe);-2))' => UrmHe=1.5E-11
//mμ constant; *Muon mass - Measurement
    "mμ",       "0.1134289257_u",
    "Usmμ",     "0.0000000025_u",
    "Urmμ",     "'→NUM(ROUND(UBASE(ⒸUsmμ/Ⓒmμ);-2))'",
  // mμ=0.1134289257_u Usmμ=0.0000000025_u '→NUM(ROUND(UBASE(Usmμ/mμ);-2))' => Urmμ=2.2E-8
//mτ constant; *Tau mass - Measurement
    "mτ",       "1.90754_u",
    "Usmτ",     "0.00013_u",
    "Urmτ",     "'→NUM(ROUND(UBASE(ⒸUsmμ/Ⓒmμ);-2))'",
  // mτ=1.90754_u Usmτ=0.00013_u '→NUM(ROUND(UBASE(Usmτ/mτ);-2))' => Urmτ=6.8E-5
//G0 constant; *Conductance quantum - Exact calculation
    "G0",       "'CONVERT(Ⓒqe^2/(Ⓒπ*Ⓒℏ);1_S)'",
    "UsG0",     "0_S",
    "UrG0",     "0",
  // G0='CONVERT(Ⓒqe^2/(Ⓒπ*Ⓒℏ);1_S)'=7.74809 17298 63651⁳⁻⁵ S
//Rk constant; *von Klitzing constant - Exact calculation
    "Rk",       "'CONVERT(2*Ⓒπ*Ⓒℏ/Ⓒqe^2;1_Ω)'",
    "UsRk",     "0_Ω",
    "UrRk",     "0",
  // Rk='CONVERT(2*Ⓒπ*Ⓒℏ/Ⓒqe^2;1_Ω)'=25 812.80745 93045 1 Ω
//KJ constant; *Josephson constant - Exact calculation
    "KJ",       "'CONVERT(2*Ⓒqe/Ⓒh;1_Hz/V)'",
    "UsKJ",     "0_Hz/V",
    "UrKJ",     "0",
  // KJ='CONVERT(2*Ⓒqe/Ⓒh;1_Hz/V)'-4.83597 84841 69836⁳¹⁴ Hz/V
//re constant; *Classical electron radius - Calculation from measurement
    "re",       "'→NUM(ROUND(CONVERT(Ⓒα^2*Ⓒa0;1_m);XPON(UVAL(ⒸUrre*Ⓒα^2*Ⓒa0))-XPON(UVAL(Ⓒα^2*Ⓒa0))-2))'",
    "Usre",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrre*Ⓒre);-2);1_m))'",//=>Usre='→NUM(CONVERT(ROUND(UBASE(Urre*(2.8179403205E-15_m));-2);1_m))'=1.3E-24 m
    "Urre",     "'→NUM(ROUND(3*Usα/α;-2))'",                                    // Urre≈'3*Usα/α'
  // Urre=4.7E-10 α=0.00729735256434 a0=0.0529177210544 nm => re=2.8179403205E-15_m <=>
  //'→NUM(ROUND(CONVERT(α^2*a0;1_m);XPON(UVAL(Urre*α^2*a0))-XPON(UVAL(α^2*a0))-2))'
//rp constant; *Proton charge radius - Measurement
    "rp",       "8.4075-16_m",
    "Usrp",     "0.0064-16_m",
    "Urrp",     "'→NUM(ROUND(UBASE(ABS(ⒸUsrp/Ⓒrp));-2))'",
  // rp=8.4075-16_m Usrp=0.0064-16_m '→NUM(ROUND(UBASE(ABS(Usrp/rp));-2))' => Urrp=7.6E-4
//σe constant; *Thomson cross-section - Calculation from measurement
    "σe",       "'→NUM(ROUND(CONVERT(8*Ⓒπ*Ⓒre^2/3;1_m^2);XPON(UVAL(ⒸUrσe*8*Ⓒπ*Ⓒre^2/3))-XPON(UVAL(8*Ⓒπ*Ⓒre^2/3))-2))'",
    "Usσe",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrσe*Ⓒσe);-2);1_m^2))'",//=>Usσe='→NUM(CONVERT(ROUND(UBASE(Urσe*σe);-2);1_m^2))'=6.3E-38 m↑2
    "Urσe",     "'→NUM(ROUND(6*ⒸUsα/Ⓒα;-2))'",                  // Urσe≈2*Urre='6*Usα/α';9.3E-10
  // Urσe=9.4E-10 re='(2.8179403205E-15_m)' => σe=6.6524587052E-29 m↑2  <=>
  //'→NUM(ROUND(CONVERT(8*Ⓒπ*(2.8179403205E-15_m)^2/3;1_m^2);XPON(UVAL(Urσe*8*Ⓒπ*(2.8179403205E-15_m)^2/3))-XPON(UVAL(8*Ⓒπ*(2.8179403205E-15_m)^2/3))-2))'
//μe constant; *Electron magnetic moment - Measurement
    "μe",       "'(-1)*9.2847646917E-24_J/T'",
    "Usμe",     "0.0000000029E-24_J/T",
    "Urμe",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμe/Ⓒμe));-2))'",
  // μe=-9.2847646917E-24_J/T Usμe=0.0000000029E-24_J/T '→NUM(ROUND(UBASE(ABS(Usμe/μe));-2))' => Urμe=3.1E-10
//μp constant; *Proton magnetic moment - Measurement
    "μp",       "1.41060679545E-26_J/T",
    "Usμp",     "0.00000000060E-26_J/T",
    "Urμp",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμp/Ⓒμp));-2))'",
  // μp=1.41060679545E-26_J/T Usμp=0.00000000060E-26_J/T '→NUM(ROUND(UBASE(ABS(Usμp/μp));-2))' => Urμp=4.3E-10
//μn constant; *Neutron magnetic moment - Measurement
    "μn",       "'(-1)*9.6623653E-27_J/T'",
    "Usμn",     "0.0000023E-27_J/T",
    "Urμn",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμn/Ⓒμn));-2))'",
  // μn=-9.6623653E-27_J/T Usμn=0.0000023E-27_J/T '→NUM(ROUND(UBASE(ABS(Usμn/μn));-2))' => Urμn=2.4E-7
//μμ constant; *Muon magnetic moment - Measurement
    "μμ",       "'(-1)*4.49044830E-26_J/T'",
    "Usμμ",     "0.00000010E-26_J/T",
    "Urμμ",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμμ/Ⓒμμ));-2))'",
  // μμ=-4.49044830E-26_J/T Usμμ=0.00000010E-26_J/T '→NUM(ROUND(UBASE(ABS(Usμμ/μμ));-2))' => Urμμ=2.2E-8
//ge constant; *Electron g-factor - Measurement
    "ge",       "'(-1)*2.00231930436092'",
    "Usge",     "0.00000000000036",
    "Urge",     "'→NUM(ROUND(UBASE(ABS(ⒸUsge/Ⓒge));-2))'",
  // ge=-2.00231930436092 Usge=0.00000000000036 '→NUM(ROUND(UBASE(ABS(Usge/ge));-2))' => Urge=1.8E-13
//Mpl constant; *Planck mass - Calculation from measurement
    "Mpl",      "'→NUM(ROUND(CONVERT(√(Ⓒℏ*Ⓒc/ⒸG);1_kg);XPON(UVAL(ⒸUrMpl*√(Ⓒℏ*Ⓒc/ⒸG)))-XPON(UVAL(√(Ⓒℏ*Ⓒc/ⒸG)))-2))'",
    "UsMpl",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrMpl*ⒸMpl);-2);1_kg))'",//=>UsMpl='→NUM(CONVERT(ROUND(UBASE(UrMpl*Mpl);-2);1_kg))'=2.4E-13 kg
    "UrMpl",    "'ⒸUrG/2'",                                 // UrMpl=1.1E-5
  // UrMpl=1.1E-5 G=6.67430E-11_m^3/(s^2*kg) => Mpl=2.176434E-8 kg <=>
  //'→NUM(ROUND(CONVERT(√(Ⓒℏ*Ⓒc/G);1_kg);XPON(UVAL(UrMpl*√(Ⓒℏ*Ⓒc/G)))-XPON(UVAL(√(Ⓒℏ*Ⓒc/G)))-2))'
//Epl constant; *Planck energy - Calculation from measurement
    "Epl",      "'→NUM(ROUND(CONVERT(√(Ⓒℏ*Ⓒc^5/ⒸG);1_GeV);XPON(UVAL(ⒸUrEpl*√(Ⓒℏ*Ⓒc^5/ⒸG)))-XPON(UVAL(√(Ⓒℏ*Ⓒc^5/ⒸG)))-2))'",
    "UsEpl",    "'→NUM(ROUND(CONVERT(ROUND(UBASE(UrEpl*Epl);-2);1_GeV);-2))'",//=>UsEpl='→NUM(ROUND(CONVERT(ROUND(UBASE(UrEpl*Epl);-2);1_GeV);-2))'=1.4E14 GeV
    "UrEpl",    "'ⒸUrG/2'",                                 // UrEpl=1.1E-5 'ROUND(CONVERT(UBASE(UrEpl*Epl);1_GeV);-2)'
  // UrEpl=1.1E-5 G=6.67430E-11_m^3/(s^2*kg) => Epl=1.22089E19 GeV <=>
  //'→NUM(ROUND(CONVERT(√(Ⓒℏ*Ⓒc^5/G);1_GeV);XPON(UVAL(UrEpl*√(Ⓒℏ*Ⓒc^5/G)))-XPON(UVAL(√(Ⓒℏ*Ⓒc^5/G)))-2))'
//Lpl constant; *Planck length - Calculation from measurement
    "Lpl",      "'→NUM(ROUND(CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^3);1_m);XPON(UVAL(ⒸUrLpl*√(Ⓒℏ*ⒸG/Ⓒc^3)))-XPON(UVAL(√(Ⓒℏ*ⒸG/Ⓒc^3)))-2))'",
    "UsLpl",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrLpl*ⒸLpl);-2);1_m))'",//=>UsLpl='→NUM(CONVERT(ROUND(UBASE(UrLpl*Lpl);-2);1_m))'=1.8E-40 m
    "UrLpl",    "'ⒸUrG/2'",                                 // UrLpl=1.1E-5
  // UrLpl=1.1E-5 G=6.67430E-11_m^3/(s^2*kg) => Lpl=1.616255E-35 m <=>
  //'→NUM(ROUND(CONVERT(√(Ⓒℏ*G/Ⓒc^3);1_m);XPON(UVAL(UrLpl*√(Ⓒℏ*G/Ⓒc^3)))-XPON(UVAL(√(Ⓒℏ*G/Ⓒc^3)))-2))'
//Tpl constant; *Planck time - Calculation from measurement
    "Tpl",      "'→NUM(ROUND(CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^5);1_s);XPON(UVAL(ⒸUrTpl*√(Ⓒℏ*ⒸG/Ⓒc^5)))-XPON(UVAL(√(Ⓒℏ*ⒸG/Ⓒc^5)))-2))'",
    "UsTpl",    "'→NUM(CONVERT(ROUND(UBASE(ⒸUrTpl*ⒸTpl);-2);1_s))'",//=>UsTpl='→NUM(CONVERT(ROUND(UBASE(UrTpl*Tpl);-2);1_s))'=5.9E-49 s
    "UrTpl",    "'ⒸUrG/2'",                                 // UrTpl=1.1E-5
  // UrTpl=1.1E-5 G=6.67430E-11_m^3/(s^2*kg) UsG=0.00015E-11_m^3/(s^2*kg) => Tpl=5.391246E-44 s <=>
  //'→NUM(ROUND(CONVERT(√(Ⓒℏ*G/Ⓒc^5);1_s);XPON(UVAL(UrTpl*√(Ⓒℏ*G/Ⓒc^5)))-XPON(UVAL(√(Ⓒℏ*G/Ⓒc^5)))-2))'
//T°pl constant; *Planck temperature - Calculation from measurement
    "T°pl",     "'→NUM(ROUND(CONVERT(√((Ⓒℏ*Ⓒc^5/ⒸG))/Ⓒk;1_K);XPON(UVAL(ⒸUrT°pl*√((Ⓒℏ*Ⓒc^5/ⒸG))/Ⓒk))-XPON(UVAL(√((Ⓒℏ*Ⓒc^5/ⒸG))/Ⓒk))-2))'",
    "UsT°pl",   "'→NUM(CONVERT(ROUND(UBASE(ⒸUrT°pl*ⒸT°pl);-2);1_K))'",//=>UsT°pl='→NUM(CONVERT(ROUND(UBASE(UrT°pl*T°pl);-2);1_K))'=1.6E27 K
    "UrT°pl",   "'ⒸUrG/2'",                                 // UrT°pl=1.1E-5
  // UrT°pl=1.1E-5 G=6.67430E-11_m^3/(s^2*kg) => T°pl=1.416784E32 K <=>
  //'→NUM(ROUND(CONVERT(√((Ⓒℏ*Ⓒc^5/G))/Ⓒk;1_K);XPON(UVAL(UrT°pl*√((Ⓒℏ*Ⓒc^5/G))/Ⓒk))-XPON(UVAL(√((Ⓒℏ*Ⓒc^5/G))/Ⓒk))-2))'
//Eh constant; *Hartree energy - Calculation from measurement
    "Eh",       "'→NUM(ROUND(CONVERT(2*Ⓒh*Ⓒc*ⒸR∞;1_J);XPON(UVAL(ⒸUrEh*2*Ⓒh*Ⓒc*ⒸR∞))-XPON(UVAL(2*Ⓒh*Ⓒc*ⒸR∞))-2))'",
    "UsEh",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrEh*ⒸEh);-2);1_J))'",//=>UsEh='→NUM(CONVERT(ROUND(UBASE(UrEh*Eh);-2);1_J))'=4.8E-30 J
    "UrEh",     "'ⒸUrR∞'",                                  // UrEh=1.1E-12
  // UrEh=1.1E-12 R∞=10973731.568157_m⁻¹ => Eh=4.3597447222060E-18 J <=>
  //'→NUM(ROUND(CONVERT(2*Ⓒh*Ⓒc*R∞;1_J);XPON(UVAL(UrEh*2*Ⓒh*Ⓒc*R∞))-XPON(UVAL(2*Ⓒh*Ⓒc*R∞))-2))'
//γe constant; *Electron gyromagnetic ratio - Calculation from measurement
    "γe",       "'→NUM(ROUND(CONVERT(2*ABS(Ⓒμe)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(ⒸUrγe*2*ABS(Ⓒμe)/Ⓒℏ))-XPON(UVAL(2*ABS(Ⓒμe)/Ⓒℏ))-2))'",
    "Usγe",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrγe*Ⓒγe);-2);1_(s*T)^-1))'",//=>Usγe='→NUM(CONVERT(ROUND(UBASE(Urγe*γe);-2);1_(s*T)^-1))'=55._(s·T)⁻¹
    "Urγe",     "'ⒸUrμe'",                             // Urγe=3.1E-10
  // Urγe=3.1E-10 μe=-9.2847646917E-24_J/T => γe=1.76085962783E11_(s·T)⁻¹ <=>
  //'→NUM(ROUND(CONVERT(2*ABS(μe)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(Urγe*2*ABS(μe)/Ⓒℏ))-XPON(UVAL(2*ABS(μe)/Ⓒℏ))-2))'
//γp constant; *Proton gyromagnetic ratio - Calculation from measurement
    "γp",       "'→NUM(ROUND(CONVERT(2*ABS(Ⓒμp)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(ⒸUrγp*2*ABS(Ⓒμp)/Ⓒℏ))-XPON(UVAL(2*ABS(Ⓒμp)/Ⓒℏ))-2))'",
    "Usγp",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrγp*Ⓒγp);-2);1_(s*T)^-1))'",//=>Usγp='→NUM(CONVERT(ROUND(UBASE(Urγp*γp);-2);1_(s*T)^-1))'=0.12_(s·T)⁻¹
    "Urγp",     "'ⒸUrμp'",                             // Urγp=4.3E-10
  // Urγp=4.3E-10 μp=1.41060679545E-26_J/T => γp=267522187.08_(s·T)⁻¹ <=>
  //'→NUM(ROUND(CONVERT(2*ABS(μp)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(Urγp*2*ABS(μp)/Ⓒℏ))-XPON(UVAL(2*ABS(μp)/Ⓒℏ))-2))'
//γn constant; *Neutron gyromagnetic ratio - Calculation from measurement
    "γn",       "'→NUM(ROUND(CONVERT(2*ABS(Ⓒμn)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(ⒸUrγn*2*ABS(Ⓒμn)/Ⓒℏ))-XPON(UVAL(2*ABS(Ⓒμn)/Ⓒℏ))-2))'",
    "Usγn",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrγn*Ⓒγn);-2);1_(s*T)^-1))'",//=>Usγn='→NUM(CONVERT(ROUND(UBASE(Urγn*γn);-2);1_(s*T)^-1))'=44._(s·T)⁻¹
    "Urγn",     "'ⒸUrμn'",                             // Urγn=2.4e-7
  // Urγn=2.4E-7 μn=-9.6623653E-27_J/T => γn=183247175._(s·T)⁻¹ <=>
  //'→NUM(ROUND(CONVERT(2*ABS(μn)/Ⓒℏ;1_(s*T)^-1);XPON(UVAL(Urγn*2*ABS(μn)/Ⓒℏ))-XPON(UVAL(2*ABS(μn)/Ⓒℏ))-2))'
//Kc constant; *Quantum of circulation - Calculation from measurement
    "Kc",       "'→NUM(ROUND(CONVERT(Ⓒπ*Ⓒℏ/Ⓒme;1_m^2/s);XPON(UVAL(ⒸUrKc*Ⓒπ*Ⓒℏ/Ⓒme))-XPON(UVAL(Ⓒπ*Ⓒℏ/Ⓒme))-2))'",
    "UsKc",     "'→NUM(CONVERT(ROUND(UBASE(ⒸUrKc*ⒸKc);-2);1_m^2/s))'",//=>UsKc='→NUM(CONVERT(ROUND(UBASE(UrKc*Kc);-2);1_m^2/s))'=1.1E-13 m↑2/s
    "UrKc",     "'ⒸUrme'",                             // UrKc=3.1E-10
  // UrKc=3.1E-10 me=9.1093837139E-31_kg => Kc=3.6369475467S-4 m↑2/s <=>
  //'→NUM(ROUND(CONVERT(Ⓒπ*Ⓒℏ/me;1_m^2/s);XPON(UVAL(UrKc*Ⓒπ*Ⓒℏ/me))-XPON(UVAL(Ⓒπ*Ⓒℏ/me))-2))'
//θw constant; *Weak mixing angle - Measurement
    "θw",       "'→NUM(ROUND(CONVERT(ASIN(√(0.22305));1_°);XPON(UVAL(ⒸUrθw*ASIN(√(0.22305))))-XPON(UVAL(ASIN(√(0.22305))))-2))'",
    "Usθw",     "'→NUM(CONVERT(ROUND((ASIN(√(0.22305+0.00023))-ASIN(√(0.22305-0.00023)))/2;-2);1_°))'",//Usθw=0.016 °
    "Urθw",     "'→NUM(ROUND(UBASE(ABS(ⒸUsθw/CONVERT(ASIN(√(0.22305));1_°)));-2))'",//Urθw='→NUM(ROUND(UBASE(ABS(Usθw/CONVERT(ASIN(√(0.22305));1_°)));-2))'=0.00057
  // θw='CONVERT(ASIN(√(0.22305));1_°)' => θw=28.18250 85537 °  Urθw=0.00057 θw=28.183 °
  //'→NUM(ROUND(CONVERT(ASIN(√(0.22305));1_°);XPON(UVAL(Urθw*ASIN(√(0.22305))))-XPON(UVAL(ASIN(√(0.22305))))-2))'
//G0F constant; *Fermi reduced coupling constant - Measurement
    "G0F",       "1.1663787E-5_GeV^-2",
    "UsG0F",     "0.0000006E-5_GeV^-2",
    "UrG0F",     "'→NUM(ROUND(UBASE(ABS(ⒸUsG0F/ⒸG0F));-2))'",
  // G0F=1.1663787E-5_GeV^-2 UsG0F=0.0000006E-5_GeV^-2 '→NUM(ROUND(UBASE(ABS(UsG0F/G0F));-2))' => UrG0F=5.1E-7
//ΔfCs constant; *Cs hyperfine transition - Exact definition
    "ΔfCs",     "9192631770_Hz",
    "UsΔfCs",   "0_Hz",
    "UrΔfCs",   "0",

    // ------------------------------------------------------------------------
    //  Computing
    // ------------------------------------------------------------------------
    "Computing",   nullptr,

    "No",                       "False",                // No value = false
    "Yes",                      "True",                 // Yes value = true
    "UnixEpoch",                "19700101_date",
    "SinclairZX81RAM",          "1_KiB",
    "PageSize",                 "4_KiB",
    "HelloWorld",               "\"Hello World\""
};
//   clang-format on


static runtime &invalid_constant_error()
// ----------------------------------------------------------------------------
//    Return the error message for invalid constants
// ----------------------------------------------------------------------------
{
    return rt.invalid_constant_error();
}


static bool show_builtin_constants()
// ----------------------------------------------------------------------------
//   Check if we show the builtin constants
// ----------------------------------------------------------------------------
{
    return Settings.ShowBuiltinConstants();
}


const constant::config constant::constants =
// ----------------------------------------------------------------------------
//  Define the configuration for the constants
// ----------------------------------------------------------------------------
{
    .menu_help      = " Constants",
    .help           = " Constant",
    .prefix         = L'Ⓒ',
    .type           = ID_constant,
    .first_menu     = ID_ConstantsMenu00,
    .last_menu      = ID_ConstantsMenu99,
    .name           = ID_ConstantName,
    .value          = ID_ConstantValue,
    .command        = ID_object,
    .file           = "config/constants.csv",
    .library        = "library",
    .builtins       = basic_constants,
    .nbuiltins      = sizeof(basic_constants) / sizeof(*basic_constants),
    .error          = invalid_constant_error,
    .label          = nullptr,
    .show_builtins  = show_builtin_constants
};


object::result constant::do_parsing(config_r cfg, parser &p)
// ----------------------------------------------------------------------------
//    Try to parse this as a constant
// ----------------------------------------------------------------------------
{
    utf8    source = p.source;
    size_t  max    = p.length;
    size_t  parsed = 0;

    // First character must be a constant marker
    unicode cp = utf8_codepoint(source);
    if (cp != cfg.prefix)
        return SKIP;
    parsed = utf8_next(source, parsed, max);
    size_t first = parsed;

    // Other characters must be alphabetic
    while (parsed < max && is_valid_in_constant_name(source + parsed))
        parsed = utf8_next(source, parsed, max);
    if (parsed <= first)
        return SKIP;

    size_t     len = parsed - first;
    constant_p cst = do_lookup(cfg, source + first, len, true);
    p.length       = parsed;
    p.out          = cst;
    return cst ? OK : ERROR;
}


size_t constant::do_rendering(config_r cfg, constant_p o, renderer &r)
// ----------------------------------------------------------------------------
//   Rendering of a constant
// ----------------------------------------------------------------------------
{
    constant_g cst = o;
    size_t     len = 0;
    utf8       txt = nullptr;
    txt = cst->do_name(cfg, &len);
    if (r.editing())
        r.put(cfg.prefix);
    r.put(txt, len);
    return r.size();
}


constant_p constant::do_lookup(config_r cfg, utf8 txt, size_t len, bool error)
// ----------------------------------------------------------------------------
//   Scan the table and file to see if there is matching constant
// ----------------------------------------------------------------------------
{
    if (unit::mode)
        return nullptr;

    unit_file cfile(cfg.file);
    size_t    maxb     = cfg.nbuiltins;
    auto      builtins = cfg.builtins;
    cstring   ctxt     = nullptr;
    size_t    clen     = 0;
    uint      idx      = 0;

    // Check in-file constants
    if (cfile.valid())
    {
        cfile.seek(0);
        while (symbol_g category = cfile.next(true))
        {
            while (symbol_p name = cfile.next(false))
            {
                ctxt = cstring(name->value(&clen));

                // Constant name comparison is case-sensitive
                if (len == clen && memcmp(txt, ctxt, len) == 0)
                    return constant::make(cfg.type, idx);
                idx++;
            }
        }
    }

    // Check built-in constants
    for (size_t b = 0; b < maxb; b += 2)
    {
        if (builtins[b+1] && *builtins[b+1])
        {
            ctxt = builtins[b];
            if (ctxt[len] == 0 && memcmp(ctxt, txt, len) == 0)
                return constant::make(cfg.type, idx);
            idx++;
        }
    }

    if (error)
        cfg.error().source(txt, len);
    return nullptr;
}


utf8 constant::do_name(config_r cfg, size_t *len) const
// ----------------------------------------------------------------------------
//   Return the name for the constant
// ----------------------------------------------------------------------------
{
    unit_file cfile(cfg.file);
    size_t    maxb     = cfg.nbuiltins;
    auto      builtins = cfg.builtins;
    cstring   ctxt     = nullptr;
    uint      idx      = index();

    // Check in-file constants
    if (cfile.valid())
    {
        cfile.seek(0);
        while (symbol_g category = cfile.next(true))
        {
            while (symbol_p sym = cfile.next(false))
            {
                if (!idx)
                    return sym->value(len);
                idx--;
            }
        }
    }

    // Check built-in constants
    for (size_t b = 0; b < maxb; b += 2)
    {
        if (builtins[b+1] && *builtins[b+1])
        {
            ctxt = builtins[b];
            if (!idx)
            {
                if (len)
                    *len = strlen(ctxt);
                return utf8(ctxt);
            }
            idx--;
        }
    }
    return nullptr;
}


object_p constant::do_value(config_r cfg) const
// ----------------------------------------------------------------------------
//   Lookup a built-in constant
// ----------------------------------------------------------------------------
{
    unit_file cfile(cfg.file);
    size_t    maxb     = cfg.nbuiltins;
    auto      builtins = cfg.builtins;
    symbol_g  csym     = nullptr;
    symbol_g  cname    = nullptr;
    size_t    clen     = 0;
    uint      idx      = index();

    // Check in-file constants
    if (cfile.valid())
    {
        cfile.seek(0);
        while (symbol_g category = cfile.next(true))
        {
            uint position = cfile.position();
            while (symbol_p sym = cfile.next(false))
            {
                if (!idx)
                {
                    cname = sym;
                    utf8 ctxt = sym->value(&clen);
                    cfile.seek(position);
                    csym = cfile.lookup(ctxt, clen, false, false);
                    break;
                }
                position = cfile.position();
                idx--;
            }
            if (csym)
                break;
        }
    }

    // Check built-in constants
    for (size_t b = 0; !csym && b < maxb; b += 2)
    {
        if (builtins[b+1] && *builtins[b+1])
        {
            if (!idx)
            {
                cname = symbol::make(builtins[b]);
                csym = symbol::make(builtins[b+1]);
                break;
            }
            idx--;
        }
    }

    // If we found a definition, use that
    if (csym)
    {
        // Special cases for pi and e where we have built-in constants
        if (cname->matches("π"))
            return decimal::pi();
        else if (cname->matches("e"))
            return decimal::e();

        utf8 cdef = csym->value(&clen);
        if (*cdef == '=')
        {
            if (text_g filename = clen>1 ? text::make(cdef+1, clen-1) : cname)
                if (files_g disk = files::make(cfg.library))
                    if (object_p obj = disk->recall(filename))
                        return obj;
        }
        else
        {
            error_save esave;
            if (object_p obj = object::parse(cdef, clen))
                return obj;
        }
    }
    cfg.error();
    return nullptr;
}


utf8 constant::do_instance_help(constant::config_r cfg) const
// ----------------------------------------------------------------------------
//   Generate the help topic for a given constant menu
// ----------------------------------------------------------------------------
{
    static char buf[64];
    size_t len = 0;
    utf8 base = do_name(cfg, &len);
    snprintf(buf, sizeof(buf), "%.*s%s", int(len), base, cfg.help);
    return utf8(buf);
}





// ============================================================================
//
//   Build a constants menu
//
// ============================================================================

utf8 constant_menu::do_name(constant::config_r cfg, id type, size_t &len)
// ----------------------------------------------------------------------------
//   Return the name associated with the type
// ----------------------------------------------------------------------------
{
    uint count = type - cfg.first_menu;
    unit_file cfile(cfg.file);

    // List all preceding entries
    if (cfile.valid())
        while (symbol_p mname = cfile.next(true))
            if (*mname->value() != '=')
                if (!count--)
                    return mname->value(&len);

    if (cfg.show_builtins())
    {
        size_t maxb     = cfg.nbuiltins;
        auto   builtins = cfg.builtins;
        for (size_t b = 0; b < maxb; b += 2)
        {
            if (!builtins[b+1] || !*builtins[b+1])
            {
                if (!count--)
                {
                    len = strlen(builtins[b]);
                    return utf8(builtins[b]);
                }
            }
        }
    }

    return nullptr;
}


bool constant_menu::do_submenu(constant::config_r cfg, menu_info &mi) const
// ----------------------------------------------------------------------------
//   Load the menu from a file
// ----------------------------------------------------------------------------
{
    // Use the constants loaded from the constants file
    unit_file cfile(cfg.file);
    size_t    matching = 0;
    uint      position = 0;
    uint      count    = 0;
    id        type     = this->type();
    id        menu     = cfg.first_menu;
    id        lastm    = cfg.last_menu;
    size_t    first    = 0;
    size_t    last     = cfg.nbuiltins;

    if (cfile.valid())
    {
        while (symbol_p mname = cfile.next(true))
        {
            if (*mname->value() == '=')
                continue;
            if (menu == type)
            {
                position = cfile.position();
                while (cfile.next(false))
                    matching++;
                menu = id(menu + 1);
                break;
            }
            menu = id(menu + 1);
            if (menu > lastm)
                break;
        }
    }

    // Disable built-in constants if we loaded a file
    if (!matching || cfg.show_builtins())
    {
        bool   found    = false;
        auto   builtins = cfg.builtins;
        size_t maxb     = cfg.nbuiltins;

        for (size_t b = 0; b < maxb; b += 2)
        {
            if (!builtins[b + 1] || !*builtins[b + 1])
            {
                if (found)
                {
                    last = b;
                    break;
                }
                if (menu == type)
                {
                    found = true;
                    first = b + 2;
                }
                menu = id(menu + 1);
                if (menu > lastm)
                    break;
            }
        }
        if (found)
            count = (last - first) / 2;
    }

    items_init(mi, count + matching, 2, 1);

    // Insert the built-in constants after the ones from the file
    uint skip     = mi.skip;
    uint planes   = 1 + !!cfg.value + !!cfg.command;
    id   ids[3]   = { cfg.name, cfg.value, cfg.command };
    auto builtins = cfg.builtins;
    for (uint plane = 0; plane < planes; plane++)
    {
        mi.plane  = plane;
        mi.planes = plane + 1;
        mi.index  = plane * ui.NUM_SOFTKEYS;
        mi.skip   = skip;
        id type = ids[plane];

        if (matching)
        {
            cfile.seek(position);
            if (plane == 1)
            {
                while (symbol_g mentry = cfile.next(false))
                {
                    uint posafter = cfile.position();
                    size_t mlen = 0;
                    utf8 mtxt = mentry->value(&mlen);
                    cfile.seek(position);
                    mentry = cfile.lookup(mtxt, mlen, false, false);
                    if (cfg.label)
                        mentry = cfg.label(mentry);
                    cfile.seek(posafter);
                    if (mentry)
                        items(mi, mentry, type);
                }
            }
            else
            {
                while (symbol_g mentry = cfile.next(false))
                    items(mi, mentry, type);
            }
        }
        for (uint i = 0; i < count; i++)
        {
            cstring   label = builtins[first + 2 * i + plane % 2];
            if (plane == 1 && cfg.label)
            {
                symbol_g mentry = symbol::make(label);
                mentry = cfg.label(mentry);
                items(mi, mentry, type);
            }
            else
            {
                items(mi, label, type);
            }
        }
    }

    return true;
}


utf8 constant_menu::do_menu_help(constant::config_r cfg,
                                 constant_menu_p    cst) const
// ----------------------------------------------------------------------------
//   Generate the help topic for a given constant menu
// ----------------------------------------------------------------------------
{
    static char buf[64];
    size_t len = 0;
    utf8 base = do_name(cfg, cst->type(), len);
    snprintf(buf, sizeof(buf), "%.*s%s", int(len), base, cfg.menu_help);
    return utf8(buf);
}


bool constant::do_collection_menu(constant::config_r cfg, menu_info &mi)
// ----------------------------------------------------------------------------
//   Build the collection menu for the given config
// ----------------------------------------------------------------------------
{
    uint      infile   = 0;
    uint      count    = 0;
    uint      maxmenus = cfg.last_menu - cfg.first_menu;
    size_t    maxb     = cfg.nbuiltins;
    auto      builtins = cfg.builtins;
    unit_file cfile(cfg.file);

    // List all menu entries in the file (up to 100)
    if (cfile.valid())
        while (symbol_p mname = cfile.next(true))
            if (*mname->value() != '=')
                if (infile++ >= maxmenus)
                    break;

    // Count built-in constant menu titles
    if (!infile || cfg.show_builtins())
    {
        for (size_t b = 0; b < maxb; b += 2)
            if (!builtins[b+1] || !*builtins[b+1])
                count++;
        if (infile + count > maxmenus)
            count = maxmenus - infile;
    }

    menu::items_init(mi, infile + count);
    infile = 0;
    if (cfile.valid())
    {
        cfile.seek(0);
        while (symbol_p mname = cfile.next(true))
        {
            if (*mname->value() == '=')
                continue;
            if (infile >= maxmenus)
                break;
            menu::items(mi, mname, id(cfg.first_menu + infile++));
        }
    }
    if (!infile || cfg.show_builtins())
    {
        for (size_t b = 0; b < maxb; b += 2)
        {
            if (!builtins[b+1] || !*builtins[b+1])
            {
                if (infile >= maxmenus)
                    break;
                menu::items(mi, builtins[b], id(cfg.first_menu + infile++));
            }
        }
    }

    return true;
}



// ============================================================================
//
//   Constant-related commands
//
// ============================================================================

constant_p constant::do_key(config_r cfg, int key)
// ----------------------------------------------------------------------------
//   Return a softkey label as a constant value
// ----------------------------------------------------------------------------
{
    size_t   len = 0;
    if (utf8 txt = ui.label_for_function_key(key, &len))
        return do_lookup(cfg, txt, len, true);
    return nullptr;
}


object::result constant::lookup_command(config_r cfg, bool numerical)
// ----------------------------------------------------------------------------
//   Process a command that looks up in the given config (e.g. CONST)
// ----------------------------------------------------------------------------
{
    object_p name = rt.top();
    id ty = name->type();
    if (symbol_p sym = name->as_quoted<symbol>())
    {
        name = sym;
        ty = name->type();
    }
    if (ty != ID_symbol && ty != ID_text)
    {
        rt.type_error();
        return ERROR;
    }

    size_t      len    = 0;
    utf8        txt    = text_p(name)->value(&len);
    if (constant_p cst = constant::do_lookup(cfg, txt, len, false))
    {
        if (object_p value = cst->do_value(cfg))
        {
            if (numerical)
            {
                if (expression_p expr = value->as<expression>())
                {
                    value = expr->evaluate();
                }
                else if (unit_p u = unit::get(value))
                {
                    if (algebraic_g expr = u->value()->as<expression>())
                    {
                        object_g gcvalue = value;
                        algebraic_g uexpr = u->uexpr();
                        if (algebraic::to_decimal(expr))
                            gcvalue = unit::simple(expr, uexpr);
                        value = gcvalue;
                    }
                }
            }
            if (rt.top(value))
                return OK;
        }
    }

    cfg.error();
    return ERROR;
}


object_p constant::lookup_menu(config_r cfg, utf8 name, size_t len)
// ----------------------------------------------------------------------------
//   Find the menu in the current configuratoin
// ----------------------------------------------------------------------------
{
    unit_file cfile(cfg.file);
    size_t    maxb     = cfg.nbuiltins;
    auto      builtins = cfg.builtins;
    uint      idx      = cfg.first_menu;

    // Check in-file constants
    if (cfile.valid())
    {
        cfile.seek(0);
        while (symbol_g category = cfile.next(true))
        {
            size_t clen = 0;
            utf8 ctxt = category->value(&clen);
            if (len == clen && memcmp(name, ctxt, len) == 0)
                return object::static_object(id(idx));
            idx++;
        }
    }

    // Check built-in constants
    for (size_t b = 0; b < maxb; b += 2)
    {
        if (!builtins[b+1] || !*builtins[b+1])
        {
            cstring ctxt = builtins[b];
            if (ctxt[len] == 0 && memcmp(ctxt, name, len) == 0)
                return object::static_object(id(idx));
            idx++;
        }
    }

    return nullptr;
}


object_p constant::lookup_menu(config_r cfg, cstring name)
// ----------------------------------------------------------------------------
//   Find the menu in the current configuratoin
// ----------------------------------------------------------------------------
{
    return lookup_menu(cfg, utf8(name), strlen(name));
}
