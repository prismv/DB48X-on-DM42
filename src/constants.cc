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

//*Avogadro's number - Definition
    "NA",       "6.02214076E23_mol⁻¹",     
    "UsNA",     "0_mol⁻¹",
    "UrNA",     "0",
//*Boltzmann - Definition
    "k",        "1.380649E-23_J/K",        
    "Usk",      "0_J/K",
    "Urk",      "0",
//*Molar volume - Calculation
    "Vm",       "'CONVERT(ⒸR*ⒸStdT/ⒸStdP;1_m^3/mol)'",
    "UsVm",     "0_m^3/mol",
    "UrVm",     "0",
//*Universal gas constant - Calculation
    "R",        "'CONVERT(ⒸNA*Ⓒk;1_J/(mol*K))'",    
    "UsR",      "0_J/(mol*K)",
    "UrR",      "0",
//*Standard temperature - Definition
    "StdT",     "273.15_K",             
    "UsStdT",   "0_K",
    "UrStdT",   "0",
//*Standard pressure - Definition
    "StdP",     "101.325_kPa",          
    "UsStdP",   "0_kPa",
    "UrStdP",   "0",
//*Stefan-Boltzmann - Calculation
    "σ",        "'CONVERT(Ⓒπ^2/60*Ⓒk^4/(Ⓒℏ^3*Ⓒc^2);1_W/(m^2*K^4))'", 
    "Usσ",      "0_W/(m^2*K^4)",
    "Urσ",      "0",

    // ------------------------------------------------------------------------
    //   Physics Constants Values & Uncertainties
    // ------------------------------------------------------------------------

    "Physics",     nullptr,

// Imaginary unit in physics - Definition
    "ⅉ",         "0+ⅈ1",                 
//*Speed of light - Definition
    "c",        "299792458_m/s",        
    "Usc",      "0_m/s",
    "Urc",      "0",
//*Vaccuum permittivity - Calculation
    "ε0",       "'→NUM(ROUND(CONVERT(1/(Ⓒμ0*Ⓒc^2);1_F/m);FLOOR(LOG10(UVAL(Urα*CONVERT(1/(Ⓒμ0*Ⓒc^2);1_F/m))))-FLOOR(LOG10(UVAL(CONVERT(1/(Ⓒμ0*Ⓒc^2);1_F/m))))-2))'",
    "Usε0",     "'→NUM(ROUND(UBASE(ⒸUrε0*Ⓒε0);-2))'",//=>Usε0='→NUM(CONVERT(ROUND(UBASE(Urα*ε0);-2);1_F/m))'=1.4E-21 F/m
    "Urε0",     "'ⒸUrα'",
  // α=0.00729735256434 Urα=1.6E-10 μ0=1.25663706127E-6 H/m => ε0=8.85418 78188E-12 F/m <=>
  //'→NUM(ROUND(CONVERT(1/(μ0*Ⓒc^2);1_F/m);FLOOR(LOG10(UVAL(Urα*CONVERT(1/(μ0*Ⓒc^2);1_F/m))))-FLOOR(LOG10(UVAL(CONVERT(1/(μ0*Ⓒc^2);1_F/m))))-2))'
//*Vaccuum permeability - Calculation
    "μ0",       "'→NUM(ROUND(CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m);FLOOR(LOG10(UVAL(ⒸUrα*CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m))))-FLOOR(LOG10(UVAL(CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m))))-2))'",
    "Usμ0",     "'→NUM(ROUND(UBASE(ⒸUrμ0*Ⓒμ0);-2))'",//=>Usμ0='→NUM(CONVERT(ROUND(UBASE(Urα*μ0);-2);1_H/m))'=2.0E-16 H/m
    "Urμ0",     "'ⒸUrα'",
  // α=0.00729735256434 Urα=1.6E-10 => μ0=1.25663706127E-6 H/m <=>
  //'→NUM(ROUND(CONVERT(4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m);FLOOR(LOG10(UVAL(Urα*CONVERT(4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m))))-FLOOR(LOG10(UVAL(CONVERT(4*Ⓒπ*α*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m))))-2))'
//*Acceleration of Earth gravity - Definition
    "g",        "9.80665_m/s²",            
    "Usg",      "0_m/s²",
    "Urg",      "0",
//*Gravitation constant - Measurement
    "G",        "6.67430E-11_m^3/(s^2*kg)",
    "UsG",      "0.00015E-11_m^3/(s^2*kg)",
    "UrG",      "'→NUM(ROUND(UBASE(ⒸUsG/ⒸG);-2))'",
  // G=6.67430E-11_m^3/(s^2*kg) UsG=0.00015E-11_m^3/(s^2*kg) '→NUM(ROUND(UBASE(UsG/G);-2))' => UrG=2.2E-5
//*Planck - Definition
    "h",        "6.62607015E-34_J*s",      
    "Ush",      "0_J*s",
    "Urh",      "0",
//*Dirac - Definition
    "ℏ",        "'CONVERT(Ⓒh/(2*Ⓒπ);1_J*s)'",   
    "Usℏ",      "0_J*s",
    "Urℏ",      "0",
//*Electronic charge - Definition
    "qe",       "1.602176634E-19_C",       
    "Usqe",     "0_C",
    "Urqe",     "0",
//*Electron mass - Measurement
    "me",       "9.1093837139E-31_kg",     
    "Usme",     "0.0000000028E-31_kg",
    "Urme",     "'→NUM(ROUND(UBASE(ⒸUsme/Ⓒme);-2))'",
  // me=9.1093837139E-31_kg Usme=0.0000000028E-31_kg '→NUM(ROUND(UBASE(Usme/me);-2))' => Urme=3.1E-10
//*Neutron mass - Measurement
    "mn",       "1.67492750056E-27_kg",    
    "Usmn",     "0.00000000085E-27_kg",
    "Urmn",     "'→NUM(ROUND(UBASE(ⒸUsmn/Ⓒmn);-2))'",
  // mn=1.67492750056E-27_kg Usmn=0.00000000085E-27_kg '→NUM(ROUND(UBASE(Usmn/mn);-2))' => Urmn=5.1E-10
//*Proton mass - Measurement
    "mp",       "1.67262192595E-27_kg",    
    "Usmp",     "0.00000000052E-27_kg",
    "Urmp",     "'→NUM(ROUND(UBASE(ⒸUsmp/Ⓒmp);-2))'",
  // mp=1.67262192595E-27_kg Usmp=0.00000000052E-27_kg '→NUM(ROUND(UBASE(Usmp/mp);-2))' => Urmp=3.1E-10
//*Hydrogen mass - Measurement
    "mH",       "1.00782503223_u",         
    "UsmH",     "0.00000000009_u",
    "UrmH",     "'→NUM(ROUND(UBASE(ⒸUsmH/ⒸmH);-2))'",
  // mH=1.00782503223_u UsmH=0.00000000009_u '→NUM(ROUND(UBASE(UsmH/mH);-2))' => UrmH=8.9E-11
//*Mass unit - Measurement
    "u",        "1.66053906892E-27_kg",    
    "Usu",      "0.00000000052E-27_kg",
    "Uru",      "'→NUM(ROUND(UBASE(ⒸUsu/Ⓒu);-2))'",
  // u=1.66053906892E-27_kg Usu=0.00000000052E-27_kg '→NUM(ROUND(UBASE(Usu/u);-2))' => Uru=3.1E-10
//*Mass unit (Dalton) - Measurement
    "Da",       "1.66053906892E-27_kg",    
    "UsDa",     "0.00000000052E-27_kg",
    "UrDa",     "'→NUM(ROUND(UBASE(ⒸUsDa/ⒸDa);-2))'",
  // Da=1.66053906892E-27_kg UsDa=0.00000000052E-27_kg '→NUM(ROUND(UBASE(UsDa/Da);-2))' => UrDa=3.1E-10
//*q/me ratio - Calculation
    "qme",      "'CONVERT(Ⓒqe/Ⓒme;1_C/kg)'",  
    "Usqme",    "??",
    "Urqme",    "'ⒸUrme'",
//*mp/me ratio - Calculation
    "mpme",     "'Ⓒmp/Ⓒme'",             
    "Usmpme",   "??",
    "Urmpme",   "'ⒸUrme+ⒸUrmp'",                                  // => Urmpme=6.2E-10
//*fine structure - Measurement
    "α",        "0.00729735256434",        
    "Usα",      "0.00000000000114",
    "Urα",      "'→NUM(ROUND(UBASE(ⒸUsα/Ⓒα);-2))'",
  // α=0.00729735256434 Usα=0.00000000000114 '→NUM(ROUND(UBASE(Usα/α);-2))' => Urα=1.6E-10
//*Magnetic flux quantum - Calculation
    "ø",        "'CONVERT(Ⓒπ*Ⓒℏ/Ⓒqe;1_Wb)'",      
    "Usø",      "0_Wb",
    "Urø",      "0",
//*Faraday - Calculation
    "F",        "'CONVERT(ⒸNA*Ⓒqe;1_C/mol)'",      
    "UsF",      "0_C/mol",
    "UrF",      "0",
//*Rydberg - Measurement
    "R∞",       "10973731.568157_m⁻¹",    
    "UsR∞",     "0.000012_m⁻¹",
    "UrR∞",     "'→NUM(ROUND(UBASE(ⒸUsR∞/ⒸR∞);-2))'",
  // R∞=10973731.568157_m⁻¹ UsR∞=0.000012_m⁻¹ '→NUM(ROUND(UBASE(UsR∞/R∞);-2))' => UrR∞=1.1⁳⁻¹²
//*Bohr radius - Calculation
    "a0",       "'CONVERT(4*Ⓒπ*Ⓒε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2);1_nm)'",    
    "Usa0",     "??",
    "Ura0",     "'ⒸUrε0'",                                    // Ura0=1.6E-10
//*Bohr magneton - Calculation
    "μB",       "'CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒme);1_J/T)'",  
    "UsμB",     "??",
    "UrμB",     "'ⒸUrme'",                                    // UrμB=3.1E-10
//*Nuclear magneton - Calculation
    "μN",       "'CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒmp);1_J/T)'",  
    "UsμN",     "??",
    "UrμN",     "'ⒸUrmp'",                                    // UrμN=3.1E-10
//*Photon wavelength - Calculation
    "λ0",       "'CONVERT(Ⓒh*Ⓒc/Ⓒqe/(1_V);1_nm)'",  
    "Usλ0",     "0_nm",
    "Urλ0",     "0",
//*Photon frequency - Calculation
    "f0",       "'CONVERT(Ⓒc/Ⓒλ0;1_Hz)'",            
    "Usf0",     "0_Hz",
    "Urf0",     "0",
//*Electron Compton wavelength - Calculation
    "λc",       "'CONVERT(Ⓒh/(Ⓒme*Ⓒc);1_nm)'",      
    "Usλc",     "??",
    "Urλc",     "'ⒸUrme'",                                   // Urλc=3.1E-10
//*Proton Compton wavelength - Calculation
    "λpc",      "'CONVERT(Ⓒh/(Ⓒmp*Ⓒc);1_nm)'",      
    "Usλpc",    "??",
    "Urλpc",    "'ⒸUrmp'",                                   // Urλpc=3.1E-10
//*Neutron Compton wavelength - Calculation
    "λnc",      "'CONVERT(Ⓒh/(Ⓒmn*Ⓒc);1_nm)'",      
    "Usλnc",    "??",
    "Urλnc",    "'ⒸUrmn'",                                   // Urλnc=5.1E-10
//*Wien's constant - Theory
    "c3",       "2.897771955185172661_mm*K",       
    "Usc3",     "0_mm*K",
    "Urc3",     "0",
//* k/q - Calculation
    "kq",       "'CONVERT(Ⓒk/Ⓒqe;1_J/(K*C))'",   
    "Uskq",     "0_J/(K*C)",
    "Urkq",     "0",
//* ε0/q - Calculation
    "ε0q",      "'CONVERT(Ⓒε0/Ⓒqe;1_F/(m*C))'",  
    "Usε0q",    "??",
    "Urε0q",    "'Urε0'",                                    // Urε0q=1.6E-10
//* q*ε0 - Calculation
    "qε0",      "'CONVERT(Ⓒqe*Ⓒε0;1_F*C/m)'",    
    "Usqε0",    "??",
    "Urqε0",    "'Urε0'",                                    // Urqε0=1.6e-10
//*Coulomb constant=calc.
    "Kclb",     "'CONVERT(1/(4*Ⓒπ*Ⓒε0);1_(N*(m/C)^2))'",   
    "UsKclb",   "??",
    "UrKclb",   "'Urε0'",                                    // UrKclb=1.6e-10
// Dielectric constant - Definition
    "εsi",      "11.9",                 
    "Usεsi",    "0",
    "Urεsi",    "0",
// SiO2 dielectric constant - Definition
    "εox",      "3.9",                  
    "Usεox",    "0",
    "Urεox",    "0",
//*Ref intensity - Definition
    "I0",       "0.000000000001_W/m^2", 
    "UsI0",     "0_W/m^2",
    "UrI0",     "0",
//*Vacuum characteristic impedance - Calculation
    "Z0",       "'CONVERT(Ⓒμ0*Ⓒc;1_Ω)'",   
    "UsZ0",     "??",
    "UrZ0",     "ⒸUrμ0",                                    // UrZ0=1.6e-10
//*Deuterium mass - Measurement
    "mD",       "2.01410177812_u",           
    "UsmD",     "0.00000000012_u",
    "UrmD",     "'→NUM(ROUND(UBASE(ⒸUsmD/ⒸmD);-2))'",
  // mD=2.01410177812_u UsmD=0.00000000012_u '→NUM(ROUND(UBASE(UsmD/mD);-2))' => UrmD=6.0E-11
//*Tritium mass - Measurement
    "mT",       "3.0160492779_u",            
    "UsmT",     "0.0000000024_u",
    "UrmT",     "'→NUM(ROUND(UBASE(ⒸUsmT/ⒸmT);-2))'",
  // mT=3.0160492779_u UsmT=0.0000000024_u '→NUM(ROUND(UBASE(UsmT/mT);-2))' => UrmT=8.0E-10
//*Helium atomic mass - Measurement
    "mHe",      "4.00260325413_u",           
    "UsmHe",    "0.00000000006_u",
    "UrmHe",    "'→NUM(ROUND(UBASE(ⒸUsmHe/ⒸmHe);-2))'",
  // mHe=4.00260325413_u UsmHe=0.00000000006_u '→NUM(ROUND(UBASE(UsmHe/mHe);-2))' => UrmHe=1.5E-11
//*Conductance quantum - Calculation
    "G0",       "'CONVERT(Ⓒqe^2/(Ⓒπ*Ⓒℏ);1_S)'",  
    "UsG0",     "0_S",
    "UrG0",     "0",
//*von Klitzing constant - Calculation
    "Rk",       "'CONVERT(2*Ⓒπ*Ⓒℏ/Ⓒqe^2;1_Ω)'",  
    "UsRk",     "0_Ω",
    "UrRk",     "0",
//*Josephson constant - Calculation
    "KJ",       "'CONVERT(2*Ⓒqe/Ⓒh;1_Hz/V)'",    
    "UsKJ",     "0_Hz/V",
    "UrKJ",     "0",
//*Classical electron radius - Calculation
    "re",       "'CONVERT(Ⓒα^2*Ⓒa0;1_m)'",       
    "Usre",     "??",
    "Urre",     "4.7E-10",                                    // Urre≈3*1.6e-10
//*Thomson cross-section - Calculation
    "σe",       "'CONVERT(8*Ⓒπ*Ⓒre^2/3;1_m^2)'", 
    "Usσe",     "??",
    "Urσe",     "9.3E-10",                                    // Urσe≈2*Urre
//*Electron magnetic moment - Measurement
    "μe",       "'(-1)*9.2847646917E-24_J/T'",    
    "Usμe",     "0.0000000029E-24_J/T",
    "Urμe",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμe/Ⓒμe));-2))'",
  // μe=-9.2847646917E-24_J/T Usμe=0.0000000029E-24_J/T '→NUM(ROUND(UBASE(ABS(Usμe/μe));-2))' => Urμe=3.1E-10
//*Proton magnetic moment - Measurement
    "μp",       "1.41060679545E-26_J/T",          
    "Usμp",     "0.00000000060E-26_J/T",
    "Urμp",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμp/Ⓒμp));-2))'",
  // μp=1.41060679545E-26_J/T Usμp=0.00000000060E-26_J/T '→NUM(ROUND(UBASE(ABS(Usμp/μp));-2))' => Urμp=4.3E-10
//*Neutron magnetic moment - Measurement
    "μn",       "'(-1)*9.6623653E-27_J/T'",       
    "Usμn",     "0.0000023E-27_J/T",
    "Urμn",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμn/Ⓒμn));-2))'",
  // μn=-9.6623653E-27_J/T Usμn=0.0000023E-27_J/T '→NUM(ROUND(UBASE(ABS(Usμn/μn));-2))' => Urμn=2.4E-7
//*Muon magnetic moment - Measurement
    "μμ",       "'(-1)*4.49044830E-26_J/T'",      
    "Usμμ",     "0.00000010E-26_J/T",
    "Urμμ",     "'→NUM(ROUND(UBASE(ABS(ⒸUsμμ/Ⓒμμ));-2))'",
  // μμ=-4.49044830E-26_J/T Usμμ=0.00000010E-26_J/T '→NUM(ROUND(UBASE(ABS(Usμμ/μμ));-2))' => Urμμ=2.2E-8
//*Electron g-factor - Measurement
    "ge",       "'(-1)*2.00231930436092'",        
    "Usge",     "0.00000000000036",
    "Urge",     "'→NUM(ROUND(UBASE(ABS(ⒸUsge/Ⓒge));-2))'",
  // ge=-2.00231930436092 Usge=0.00000000000036 '→NUM(ROUND(UBASE(ABS(Usge/ge));-2))' => Urge=1.8E-13
//*Planck mass - Calculation
    "Mpl",      "'CONVERT(√(Ⓒℏ*Ⓒc/ⒸG);1_kg)'",       
    "UsMpl",    "??",
    "UrMpl",    "'ⒸUrG/2'",                                 // UrMpl=1.1E-5
//*Planck temperature - Calculation
    "T°pl",     "'CONVERT(√((Ⓒℏ*Ⓒc^5/ⒸG))/Ⓒk;1_K)'",
    "UsT°pl",   "??",
    "UrT°pl",   "'ⒸUrG/2'",                                 // UrT°pl=1.1E-5
//*Planck length - Calculation
    "Lpl",      "'CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^3);1_m)'",      
    "UsLpl",    "??",
    "UrLpl",    "'ⒸUrG/2'",                                 // UrLpl=1.1E-5
//*Planck time - Calculation
    "Tpl",      "'CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^5);1_s)'",      
    "UsTpl",    "??",
    "UrTpl",    "'ⒸUrG/2'",                                 // UrTpl=1.1E-5
//*Hartree energy - Calculation
    "Eh",       "'CONVERT(2*Ⓒh*Ⓒc*ⒸR∞;1_J)'",        
    "UsEh",     "??",
    "UrEh",     "'ⒸUrR∞'",                                  // UrEh=1.1E-12
//*Weak mixing angle - Measurement
    "θw",       "'CONVERT(ASIN(√(0.22305));1_r)'",     
    "Usθw",     "0_r",
    "Urθw",     "0",
//*Cs hyperfine transition - Definition
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
    if (file *saving = r.file_save())
    {
        // Saving: can't have two files open at once on DMCP
        file_closer_while_writing fc(*saving);
        txt = cst->do_name(cfg, &len);
    }
    else
    {
        txt = cst->do_name(cfg, &len);
    }
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
        // Need to close the configuration file before we parse the constants
        file_closer fc(cfile);

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
                    {
                        file_closer fc(cfile);
                        mentry = cfg.label(mentry);
                    }
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
                file_closer fc(cfile);
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
