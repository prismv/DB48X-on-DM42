# Constants library

The DB48X calculator features a library of constants covering mathematics,
physics, chemistry and computer science. The built-in constants can be extended
using the `config/constants.csv` configuration file.

## Mathematics constants

### π constant

The ratio between the circumference and the diameter of a circle.

![Definition of π](img/definition_of_pi.bmp)

### e constant

Euler's constant is the base for the natural logarithm.

![Definition of e](img/definition_of_e.bmp)

### ⅈ constant

The imaginary unit, such that ⅈ²=-1.

The picture below shows imaginary unit ⅈ in the complex plane: Real numbers are
conventionally drawn on the horizontal axis, and imaginary numbers on the
vertical axis.

![Imaginary unit](img/imaginary_unit.bmp)

### ⅉ constant

An alternative notation, often preferred by physicists, for the imaginary unit ⅈ, verifying ⅉ²=-1.

### ∞ constant

Infinity is something that is larger than any natural number.
Its numerical value is an arbitrary large number that is not really infinite.

### ? constant

The undefined constant is used to represent undefined values, such as the result of undefined operations.

### rad constant

One radian.

### twoπ constant

Two π radian (one full circle).

### angl constant

Half turn in degrees.


## Chemistry constant

### NA constant

Avogradro constant is the number of constituent particles per mole. Since 
the redefinition of the mole in 2019, as being the amount of substance 
containing exactly `NA` particles, the `NA` constant is therefore defined 
as the following exact value:
```rpl
    12 SIG  NA=6.02214076E23_mol⁻¹  UsNA=0_mol⁻¹  UrNa=0
```

### k constant

The Boltzmann constant is the proportionality factor that relates the average
relative thermal energy of particles in a gas with the thermodynamic temperature
of the gas. 

Since the 2019 redefinition of the SI units, the `k` constant is exactly defined 
as the following:
```rpl
    12 SIG  k=1.38064 9E-23_J/K  Usk=0_J/K  Urk=0
```

### Vm constant

Molar volume of an ideal gas at 1 atmosphere of pressure and 0°C. By convention, 
it is chosen as the result of the following exact calculation:
```rpl
    12 SIG  Vm='CONVERT(ⒸR*ⒸStdT/ⒸStdP;1_m^3/mol)'  UsVm=0_m^3/mol  UrVm=0
```

### R constant

The universal gas constant is the molar equivalent to the Boltzmann constant,
expressed in units of energy per temperature increment per amount of substance,
rather than energy per temperature increment per particle. 

Since the 2019 redefinition of the SI units, it is chosen as the result of 
the following exact calculation:
```rpl
    12 SIG  R='CONVERT(ⒸNA*Ⓒk;1_J/(mol*K))'  UsR=0_J/(mol*K)  UrR=0
```

### StdT constant

Standard temperature as defined by IUPAC in 1982. 

Standard temperature and pressure (STP) or Standard conditions for temperature
and pressure are various standard sets of conditions for experimental
measurements used to allow comparisons to be made between different sets of
data. The most used standards are those of the International Union of Pure and
Applied Chemistry (IUPAC, used by DB48X) and the National Institute of Standards
and Technology (NIST). Other organizations have established a variety of other
definitions. By convention, it is chosen as the following exact value:
```rpl
    12 SIG  StdT=273.15_K  UsStdT=0_K  UrStdT=0
```

### StdP constant

Standard pressure as defined by IUPAC in 1982, corresponding to 1 atm.

Standard temperature and pressure (STP) or Standard conditions for temperature
and pressure are various standard sets of conditions for experimental
measurements used to allow comparisons to be made between different sets of
data. The most used standards are those of the International Union of Pure and
Applied Chemistry (IUPAC, used by DB48X) and the National Institute of Standards
and Technology (NIST). Other organizations have established a variety of other
definitions. By convention, it is chosen as the following exact value:
```rpl
    12 SIG  StdP=101.325_kPa  UsStdP=0_kPa  UrStdP=0
```

### σ constant

Stefan-Boltzmann constant, the factor of proportionality in the Stefan-Boltzmann
law describing the intensity of the thermal radiation emitted by matter in terms
of that matter's temperature.

For an ideal absorber/emitter or black body, the Stefan–Boltzmann law states
that the total energy radiated per unit surface area per unit time (also known
as the radiant exitance) is directly proportional to the fourth power of the
black body's temperature, `T`:`M°=σ·T⁴`.  

Since the redefinition of the SI units in 2019, the `σ` constant has an exact
value resulting from the following calculation:
```rpl
    12 SIG  σ='CONVERT(Ⓒπ^2/60*Ⓒk^4/(Ⓒℏ^3*Ⓒc^2);1_W/(m^2*K^4))'  Usσ=0_W/(m^2*K^4)  Urσ=0
```

#### Mu Constant

The molar mass constant is a physical constant defined as one twelfth of 
the molar mass of carbon-12: `MC12/12`. The molar mass of an element or 
compound is its relative atomic mass (or atomic weight, noted `Ar`) or 
relative molecular mass multiplied by the molar mass constant. Since 2019, 
the SI definition of mole changed such that the molar mass constant remains 
nearly but no longer exactly 1 g/mol. For internal consistency this value 
depends therefore on the mass unit `u` [u-constant](#u-constant) and is 
calculated by the following expression:
```rpl
    12 SIG  Mu='CONVERT(ⒸNA*Ⓒu;1_kg/mol)'
```
Due to its expression, the relative uncertainty of the molar mass is 
the same as the one of the mass unit `u`:
```rpl
    12 SIG  UsMu='→NUM(CONVERT(ROUND(UBASE(ⒸUrMu*ⒸMu);-2);1_kg/mol))'  UrMu='ⒸUru'
```

#### MC12 constant

Molar Mass of carbon-12. Since 1960, mole is the amount of substance of 
a system which contains as many elementary entities as there are atoms in 
12 gram of carbon-12. Since 2019, the SI definition of mole changed such 
that the molar mass of carbone-12 remains nearly but no longer exactly 
12 g/mol. For internal consistency this value depends therefore on the 
mass unit `u` [u-constant](#u-constant) and is calculated by the following 
expression:
```rpl
    12 SIG  MC12='CONVERT(12*ⒸMu;1_kg/mol)'
```
Due to its expression, the relative uncertainty of the molar mass of
carbon-12 is the same as the one of the molar mass `Mu`:
```rpl
    12 SIG  UsMC12='→NUM(CONVERT(ROUND(UBASE(ⒸUrMC12*ⒸMC12);-2);1_kg/mol))'  UrMC12='ⒸUrMu'
```

#### Are constant
Electron relative atomic mass. The experimental value of the electron 
relative atomic mass is an important constant (usually noted as 
`Ar(e)`) which is needed to calculate the unit mass value `u` 
[u-constant](#u-constant). According to COTATA2022, it is a dimensionless 
quantity which is determined iteratively by frequency measurements using 
Penning trap. Its value is measured as:
```rpl
    12 SIG  Are=5.485799090441E-4
```
Its absolute and relative uncertainties come from measurement:
```rpl
    12 SIG  UsAre=0.000000000097E-4  UrAre='→NUM(ROUND(UBASE(ⒸUsAre/ⒸAre);-2))'
```

#### n0 constant

The Loschmidt constant or Loschmidt's number is the number of particles
(atoms or molecules) of an ideal gas per volume (the number density), 
and usually quoted at standard temperature and pressure. Since 2019 with 
the redifinition of the mole, it is calculated exactly as:
```rpl
    12 SIG  n0='CONVERT(ⒸNA/ⒸVm;1_m^-3)'  Usn0=0_m^-3  Urn0=0
```

#### SoR constant

The Sakur-Tetrode constant gives the absolute entropy at an absolute 
temperature of `T=1_K` and standard atmospheric pressure `StdP=101.325_kPa` 
for one mole of an ideal monoatomic gas composed of particles of mass equal 
to the atomic mass constant. It is used in the general expression of the 
entropy of an ideal monoatomic gas of relative atomic mass `Ar` at a pressure 
`p_kPa` and a temperature `T_K` which is given by:
`S=ⒸR*(SoR+3/2*LN(Ar)−LN((p_kPa)/ⒸStdP)+5/2*LN((T_K)/(1_K)))`. 

The dimensionless `SoR` constant is calculated from the measured 
value of the unit mass `u`.
```rpl
    12 SIG  SoR='5/2+LN(UBASE(Ⓒu*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)'
```
The standard and relative uncertainties are evaluated as:
```rpl
    12 SIG  UsSoR='ROUND(UBASE(ⒸUrSoR*ABS(ⒸSoR));-2)'  UrSoR=4.0E-10
```


## Physics constants

### c constant

Speed of light in vaccuum, a universal physical constant that is exactly equal
to 299792458 metres per second (by definition of the metre).

According to the [special theory of relativity](http://en.wikipedia.org/wiki/Special_relativity),
`c` is the upper limit for the speed at which conventional matter or energy
(and thus any signal carrying information) can travel through space.

The [theory of incomplete measurements](http://physics.dinechin.org) presents
space and time as having no existence on their own. We only derive them from
*measurements* of distance and duration made using photons. In that viewpoint,
`c` is the limit of what can be measured using photons. Anything travelling
faster than light is indistinguishable from anti-matter. This is illustrated by
the thought experiment known (or not) as the *bat and the supersonic jet*.
This little tidbit is only mentioned here because this particular theory was
devised by the primary author of DB48X.

Its value is exactly defined as
```rpl
    12 SIG  c=299792458_m/s  Usc=0_m/s  Urc=0
```

### ε0 constant

Vacuum permittivity, commonly denoted `ε0` (pronounced "epsilon nought" or
"epsilon zero"), is the value of the absolute dielectric permittivity of
classical vacuum. It may also be referred to as the permittivity of free space,
the electric constant, or the distributed capacitance of the vacuum. It is an
ideal (baseline) physical constant. It is a measure of how dense of an electric
field is "permitted" to form in response to electric charges and relates the
units for electric charge to mechanical quantities such as length and force.

Its value depends on the vaccuum permeability constant `μ0` and is 
calculated with:
```rpl
    12 SIG  ε0='CONVERT(1/(Ⓒμ0*Ⓒc^2);1_F/m)'
```
Its relative uncertainty is the same as the vaccuum permeability, then:
```rpl
    12 SIG  Usε0='→NUM(ROUND(UBASE(ⒸUrε0*Ⓒε0);-2))'  Urε0='ⒸUrμ0'
```

### μ0 constant

The vacuum magnetic permeability (variously vacuum permeability, permeability of
free space, permeability of vacuum, magnetic constant) is the magnetic
permeability in a classical vacuum. It is a physical constant, conventionally
written as `μ0` (pronounced "mu nought" or "mu zero"). It quantifies the strength
of the magnetic field induced by an electric current.

Its value depends on the fine structure constant measurement `α` and is 
calculated with:
```rpl
    12 SIG  μ0='CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m)'
```
Its relative uncertainty is the same as the fine structure constant, then:
```rpl
    12 SIG  Usμ0='CONVERT(4*Ⓒπ*Ⓒα*Ⓒℏ/(Ⓒqe^2*Ⓒc);1_H/m)  Urμ0='ⒸUrα'
```

### g constant

Acceleration of Earth gravity, equivalent to the free-fall acceleration. Its 
value is defined by convention to be exactly:
```rpl
    12 SIG  g=9.80665_m/s²  Usg=0_m/s²  Urg=0
```

### G constant

The gravitational constant is an empirical physical constant involved in the
calculation of gravitational effects in Sir Isaac Newton's law of universal
gravitation and in Albert Einstein's theory of general relativity.

According to Newton's law of universal gravitation, the magnitude of the
attractive force (`F`) between two bodies each with a spherically symmetric
density distribution is directly proportional to the product of their masses, `m₁`
and `m₂`, and inversely proportional to the square of the distance, `r`, directed
along the line connecting their centres of mass:

![Newton Gravitation](img/NewtonGravitation.bmp)

Its measured value is:
```rpl
    12 SIG  G=6.67430E-11_m^3/(s^2*kg)
```
The uncertainties are evaluated as:
```rpl
    12 SIG  UsG=0.00015E-11_m^3/(s^2*kg)  UrG='→NUM(ROUND(UBASE(ⒸUsG/ⒸG);-2))'
```

### h constant

The Planck constant is a fundamental physical constant that appears in quantum
mechanics. A photon's energy `E` is equal to its frequency `ν` multiplied by the 
Planck constant (`E=h·ν`), and the wavelength `λ` of a matter wave equals the 
Planck constant divided by the associated particle momentum `p` (`λ=h/p`). 

Since the 2019 SI redefinition, it is exactly defined as:
```rpl
    12 SIG  h=6.62607015E-34_J*s  Ush=0_J*s  Urh=0  
```

### ℏ constant

The reduced Planck constant, ℏ, also known as the Dirac Constant, is defined as
`ℏ=h/2π`. It is exactly calculated accordingly as:
```rpl
    12 SIG  ℏ='CONVERT(Ⓒh/(2*Ⓒπ);1_J*s)'  Usℏ=0_J*s  Urℏ=0  
```

### qe constant

The elementary electric charge is a fundamental physical constant, defined as
the electric charge carried by a single proton or, equivalently, the magnitude
of the negative electric charge carried by a single electron, which has charge
`−qe`.

In the SI system of units, the value of the elementary charge is exactly defined
as `qe=1.602176634⁳⁻¹⁹` coulombs. Since the 2019 redefinition of SI base units,
the seven SI base units are defined by seven fundamental physical constants, of
which the elementary charge is one. As a consequence of this change, the value
of that constant in DB48X differs from the value in the HP50G, which named it 
`q`, with value `1.60217733⁳⁻¹⁹` coulombs. 

The exact value of `qe` is then:
```rpl
    12 SIG  qe=1.602176634E-19_C  Usqe=0_C  Urqe=0  
```

### me constant

Electron mass. In particle physics, the electron mass is the mass of a
stationary electron, also known as the invariant mass of the electron. It is one
of the fundamental constants of physics. Its value is closely related to the unit
mass measurement through the electron relative atomic mass: `me=u·Ar(e)` where 
`Ar(e)` [Are-constant](#Are-constant) is determined iteratively by frequency 
measurements using Penning trap. 

According to https://en.wikipedia.org/wiki/Electron_mass this is equivalent to
determine its experimental value by the following calculation where it depends 
on the fine structure constant `α` and the Rysberg constant `R∞`:
```rpl
    12 SIG  me=CONVERT(2*Ⓒh*ⒸR∞/((Ⓒα^2)*Ⓒc);1_kg)
```
Its relative uncertainty is therefore on the same order either of the sum 
`2*Urα+UrR∞`, or is the same as the relative uncertainty of the unit mass `u`:
```rpl
    12 SIG  Usme='→NUM(CONVERT(ROUND(UBASE(ⒸUrme*Ⓒme);-2);1_kg))'  Urme='ⒸUru'
```

### mp constant

Mass of the proton. Being the hydrogen H-1 nucleus, discovered and  
named by Ernest Rutherford, the proton was considered at first to be
an elementary particle. It is now recognized as being composed of two 
valence up quarks of charge `+⁠2/3qe` each, and one valence down quark
of charge `−⁠1/3⁠qe`. The rest masses of quarks contribute only about 1% 
of a proton's mass and 99% is quantum chromodynamics binding energy
including gluons field energy and quarks kinetic relativistic energy.

Precise measurements of proton mass is obtained by particle drag race
experiments. Its measured value is:
```rpl
    12 SIG  mp=1.67262192595E-27_kg
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usmp=0.00000000052E-27_kg  Urmp='→NUM(ROUND(UBASE(ⒸUsmp/Ⓒmp);-2))'
```

### mn constant

Neutron mass. Being electrically neutral, the neutron emission, wrongly 
interpreted at the beginning as some sort of γ radiation was correctly 
identified in 1932 by Chadwick as the neutron hypothesized by Rutherford
as a neutral particule having about the same mass as the proton. Free 
neutrons have a half-life of 613.9 seconds.

The value of its mass is determined experimentally by mass spectrometry as:
```rpl
    12 SIG  mn=1.67492750056E-27_kg
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usmn=0.00000000085E-27_kg  Urmn='→NUM(ROUND(UBASE(ⒸUsmn/Ⓒmn);-2))'
```

### mH constant

Mass of the neutral hydrogen atom. The electrically neutral hydrogen H-1
atom contains a single positively charged proton in the nucleus, and 
a single negatively charged electron bound to the nucleus by the Coulomb 
force. Atomic hydrogen constitutes about 75% of the baryonic mass of the 
Universe. Precise measurements of hydrogen is obtained by spectrometry. 
Its measured value is:
```rpl
    12 SIG  mH=1.00782503223_u
```
with absolute and relative uncertainties:
```rpl
    12 SIG  UsmH=0.00000000009_u  UrmH='→NUM(ROUND(UBASE(ⒸUsmH/ⒸmH);-2))'
```

### u constant

Unified atomic mass unit (AMU) or dalton, in physics and chemistry, a unit 
for expressing masses of atoms, molecules, or subatomic particles. An atomic 
mass unit is equal to 1/12 the mass of a single atom of carbon-12. Being 
affected by the bonding energy of the nucleons in the nucleus, the mass in 
daltons of an atom is in general numerically close but not exactly equal to 
the number of nucleons in its nucleus. The atomic mass unit is determined 
from the calculation of the electron rest mass `me` and the measurement of
the electron relative atomic mass `Ar(e)` [Are-constant](#Are-constant) 
(that is, the mass of electron divided by the atomic mass constant). It is 
calculated by:
```rpl
    12 SIG  u='CONVERT(Ⓒme/ⒸAre;1_kg)'
```
the relative uncertainty is evaluated as:
```rpl
    12 SIG  Usu='→NUM(CONVERT(ROUND(UBASE(ⒸUru*Ⓒu);-2);1_kg))'  Uru=3.1E-10
```

### Da constant

Dalton or unified atomic mass unit (u) design the same constant, see
Mass unit `u` [u-constant](#u-constant). It is obtained by:
```rpl
    12 SIG  Da='Ⓒu'
```
the standard and relative uncertainties are:
```rpl
    12 SIG  UsDa='ⒸUsu'  UrDa='ⒸUru'
```

### qme constant

Ratio between the electron charge `qe` and its mass `me`. 
It depends on the calculated value of the electron mass:
```rpl
    12 SIG  qme='CONVERT(Ⓒqe/Ⓒme;1_C/kg)'
```
its relative uncertainty is the same as the electron mass:
```rpl
    12 SIG  Usqme='→NUM(CONVERT(ROUND(UBASE(ⒸUrqme*Ⓒqme);-2);1_C/kg))'  Urqme='ⒸUrme'
```

### mpme constant

Ratio between the mass of the proton `mp` and the mass of the electron 
`me`. Currently, the most precise measurements of the charge-to-mass 
ratio of a proton still use a magnetic field like Thompson did, but 
rely on measuring (cyclotron) frequencies rather than deflection. It 
is a measured dimensionless quantity:
```rpl
    12 SIG  mpme=1836.152673426
```
its standard and relative uncertainties are:
```rpl
    12 SIG  Usmpme=0.000000032  Urmpme='→NUM(ROUND(UBASE(ⒸUsmpme/Ⓒmpme);-2))'
```

### α constant

Fine-structure constant. In physics, the fine-structure constant, also known as
the Sommerfeld constant, commonly denoted by α (the Greek letter alpha), is a
fundamental physical constant which quantifies the strength of the
electromagnetic interaction between elementary charged particles. It is measured 
by observing the recoil frequency of atoms, like cesium or rubidium, when they 
absorb a photon, essentially gauging how strongly the atoms recoil, which 
provides an accurate determination of the constant's value using high precision 
measurements achieved through techniques like matter-wave interferometry.

It is a dimensionless quantity, independent of the system of units used, which
is related to the strength of the coupling of an elementary charge `qe` with the
electromagnetic field. Its measured value is:
```rpl
    11 SIG  α=0.00729735256434
```
its standard and relative uncertainties are:
```rpl
    2 SIG  Usα=0.00000000000114  Urα='→NUM(ROUND(UBASE(ⒸUsα/Ⓒα);-2))'
```

### ø constant

Magnetic flux quantum. The (superconducting) magnetic flux quantum is a
combination of fundamental physical constants: the Planck constant `h` 
and the electron charge `qe`. Its value is, therefore, the same for 
any superconductor. It is exactly calculated by:
```rpl
    12 SIG  ø='CONVERT(Ⓒπ*Ⓒℏ/Ⓒqe;1_Wb)'"  Usø=0_Wb  Urø=0
```

### F constant

Faraday constant. In physical chemistry, the Faraday constant is a 
physical constant defined as the quotient of the total electric charge 
(`q`) by the amount (`n`) of elementary charge carriers in any given 
sample of matter. It is exactly calculated by:
```rpl
    12 SIG  F='CONVERT(ⒸNA*Ⓒqe;1_C/mol)'  UsF=0_C/mol  UrF=0
```

### R∞ constant

Rydberg constant. In spectroscopy, the Rydberg constant is a physical 
constant relating to the electromagnetic spectra of an atom. The constant 
first arose as an empirical fitting parameter in the Rydberg formula for 
the hydrogen spectral series, but Niels Bohr later showed that its value 
is related to more fundamental constants according to his model of the 
atom. The Rydberg constant value is inferred from measurements of atomic 
transition frequencies in three different atoms (hydrogen, deuterium, 
and antiprotonic helium). It is measured as:
```rpl
    12 SIG  R∞=10973731.568157_m⁻¹
```
The Rydberg constant is one of the most precisely determined physical 
constants, with the following standard and relative uncertainties:
```rpl
    12 SIG  UsR∞=0.000012_m⁻¹  UrR∞='→NUM(ROUND(UBASE(ⒸUsR∞/ⒸR∞);-2))'
```

### a0 constant

Bohr radius. The Bohr radius is a physical constant, approximately equal 
to the most probable distance between the nucleus and the electron in a 
hydrogen atom in its ground state. Its value depends on the vacuum electric 
permittivity `ε0` and it is calculated by:
```rpl
    12 SIG  a0='CONVERT(4*Ⓒπ*Ⓒε0*Ⓒℏ^2/(Ⓒme*Ⓒqe^2);1_nm)'
```
through `ε0`, its relative uncertainty is the same as the one of the fine 
structure constant `α`:
```rpl
    12 SIG  Usqa0='→NUM(CONVERT(ROUND(UBASE(ⒸUsα/Ⓒα*Ⓒa0);-2);1_nm))'  Urqme='ⒸUrα'
```

### μB constant

Bohr magneton. In atomic physics, the Bohr magneton is a physical constant 
and the natural unit for expressing the magnetic moment of an electron caused 
by its orbital or spin angular momentum. In SI units, the Bohr magneton 
depends on the calculated value of the electron mass `me`. It is calculated as:
```rpl
    12 SIG  μB='CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒme);1_J/T)'
```
its relative uncertainty is the same as the electron mass:
```rpl
    12 SIG  UsμB='→NUM(CONVERT(ROUND(UBASE(ⒸUrμB*ⒸμB);-2);1_J/T))'  UrμB='ⒸUrme'
```

### μN constant

The nuclear magneton is a physical constant of magnetic moment. It 
is the standard unit used to measure the magnetic dipole moment of 
atomic nuclei and nucleons (protons and neutrons), essentially acting 
as a scale to quantify their magnetic strength. Defined in SI units, 
it depends on the measured value of the proton mass `mp`. It is 
calculated as:
```rpl
    12 SIG  μN='CONVERT(Ⓒqe*Ⓒℏ/(2*Ⓒmp);1_J/T)'
```
its relative uncertainty is the same as the proton mass:
```rpl
    12 SIG  UsμN='→NUM(CONVERT(ROUND(UBASE(ⒸUrμN*ⒸμN);-2);1_J/T))'  UrμB='ⒸUrmp'
```

### λ0 constant

Photon wavelength. Photon energy can be expressed using any unit of energy 
such as the electronvolt (eV) or the Joule (J). For short wavelength sources,
researchers often discuss photon energies in units of eV (or keV for hard
X-rays) out of convenience.  The SI definition for 1 eV derives from the
[definitional value of the electron charge](#me-constant). Photon energy `E` 
in eV can be computed from wavelength `λ` in nm as: `E=λ0/λ`. It is exactly 
calculated by:
```rpl
    12 SIG  λ0='CONVERT(Ⓒh*Ⓒc/Ⓒqe/(1_V);1_nm)'  Usλ0=0_nm  Urλ0=0
```

### f0 constant

Photon frequency. This is the frequency associated to the
[photon wavelength λ0][#λ0 constant].
It is exactly calculated by:
```rpl
    12 SIG  f0='CONVERT(Ⓒc/Ⓒλ0;1_Hz)'  Usf0=0_Hz  Urf0=0
```

### λc constant

Electron Compton wavelength. The Compton wavelength is a quantum 
mechanical property of a particle, defined as the wavelength of a 
photon whose energy is the same as the rest energy of that particle 
(based on the mass–energy equivalence). The standard Compton wavelength 
`λ` of a particle of mass `m` is given by `λ=h/(m·c)`. Since it is 
defined here for the electron, it depends on the calculated value of 
the electron mass `me` by:
```rpl
    12 SIG  λc='CONVERT(Ⓒh/(Ⓒme*Ⓒc)'
```
its relative uncertainty is the same as the electron mass:
```rpl
    12 SIG  Usλc='→NUM(CONVERT(ROUND(UBASE(ⒸUrλc*Ⓒλc);-2);1_nm))'  Urλc='ⒸUrme'
```

### λcp constant

Proton Compton wavelength. The Compton wavelength is a quantum 
mechanical property of a particle, defined as the wavelength of 
a photon whose energy is the same as the rest energy of that particle 
(based on the mass–energy equivalence). The standard Compton wavelength 
`λ` of a particle of mass `m` is given by `λ=h/(m·c)`. Since it is 
defined here for the proton, it depends on the measured value of the 
proton mass `mp` by:
```rpl
    12 SIG  λcp='CONVERT(Ⓒh/(Ⓒmp*Ⓒc);1_nm)'
```
its relative uncertainty is the same as the proton mass:
```rpl
    12 SIG  Usλcp='→NUM(CONVERT(ROUND(UBASE(ⒸUrλcp*Ⓒλcp);-2);1_nm))'  Urλcp='ⒸUrmp'
```

### λcn constant

Neutron Compton wavelength. The Compton wavelength is a quantum 
mechanical property of a particle, defined as the wavelength of 
a photon whose energy is the same as the rest energy of that particle 
(based on the mass–energy equivalence). The standard Compton wavelength 
`λ` of a particle of mass `m` is given by `λ=h/(m·c)`. Since it is 
defined here for the neutron, it depends on the measured value of the 
neutron mass `mn` by:
```rpl
    12 SIG  λcn='CONVERT(Ⓒh/(Ⓒmn*Ⓒc);1_nm)'
```
its relative uncertainty is the same as the neutron mass:
```rpl
    12 SIG  Usλcn='→NUM(CONVERT(ROUND(UBASE(ⒸUrλcn*Ⓒλcn);-2);1_nm))'  Urλcn='ⒸUrmn'
```

### λcμ constant

Muon Compton wavelength. The Compton wavelength is a quantum 
mechanical property of a particle, defined as the wavelength of 
a photon whose energy is the same as the rest energy of that particle 
(based on the mass–energy equivalence). The standard Compton wavelength 
`λ` of a particle of mass `m` is given by `λ=h/(m·c)`. Since it is 
defined here for the muon particle, it depends on the measured value of 
the muon mass `mμ` by:
```rpl
    12 SIG  λcμ='CONVERT(Ⓒh/(Ⓒmμ*Ⓒc);1_nm)'
```
its relative uncertainty is the same as the muon mass:
```rpl
    12 SIG  Usλcμ='→NUM(CONVERT(ROUND(UBASE(ⒸUrλcμ*Ⓒλcμ);-2);1_nm))'  Urλcμ='ⒸUrmμ'
```

### λcτ constant

Tau Compton wavelength. The Compton wavelength is a quantum 
mechanical property of a particle, defined as the wavelength of
a photon whose energy is the same as the rest energy of that particle 
(based on the mass–energy equivalence). The standard Compton wavelength 
`λ` of a particle of mass `m` is given by `λ=h/(m·c)`. Since it is 
defined here for the tau particle, it depends on the measured value of 
the tau mass `mτ` by:
```rpl
    12 SIG  λcτ='CONVERT(Ⓒh/(Ⓒmτ*Ⓒc);1_nm)'
```
its relative uncertainty is the same as the tau mass:
```rpl
    12 SIG  Usλcτ='→NUM(CONVERT(ROUND(UBASE(ⒸUrλcτ*Ⓒλcτ);-2);1_nm))'  Urλcτ='ⒸUrmτ'
```

### c1 constant

First radiation constant. This constant appears in the Radiance 
expression of the Planck's law: `Bλ(λ;T)=c1/λ^5/EXPM1(c2/λT)`.
It is exactly calculated by:
```rpl
    12 SIG  c1='CONVERT(2*Ⓒπ*Ⓒh*Ⓒc^2;1_(W*m^2))'  Usc1=0_(W*m^2)  Urc1=0
```

### c2 constant

Second radiation constant. This constant appears in the Radiance 
expression of the Planck's law: `Bλ(λ;T)=c1/λ^5/EXPM1(c2/λT)`.
It is exactly calculated by:
```rpl
    12 SIG  c2='CONVERT(Ⓒh*Ⓒc/Ⓒk;1_(m*K))'  Usc2=0_(m*K)  Urc2=0
```

### c3 constant

Wien's constant also knowm as the third radiation constant. In physics, Wien's 
displacement law states that the black-body radiation curve for different 
temperatures will peak at different wavelengths that are inversely proportional 
to the temperature. The shift of that peak is a direct consequence of the 
Planck radiation law, which describes the spectral brightness or intensity of 
black-body radiation as a function of wavelength at any given temperature. 
However, it had been discovered by German physicist Wilhelm Wien several years 
before Max Planck developed that more general equation, and describes the 
entire shift of the spectrum of black-body radiation toward shorter 
wavelengths as temperature increases.

Formally, the wavelength version of Wien's displacement law states that the
spectral radiance of black-body radiation per unit wavelength, peaks at the
wavelength `λpeak=c3/T` where `T` is absolute temperature. From a theoretic 
expression (using Lambert function), it is approximately given by:
```rpl
    34 SIG  c3=2.897771955185172661478605448092885_mm*K  Usc3=0_mm*K  Urc3=0
```

### kq constant

Ratio of the Boltzman constant [k-constant](#k-constant) to the
elementary charge [qe-constant](#qe-constant): `kq=k/qe`. It is 
exactly calculated by:
```rpl
    12 SIG  kq='CONVERT(Ⓒk/Ⓒqe;1_J/(K*C))'  Uskq=0_J/(K*C)  Urkq=0
```

### ε0q constant

Ratio of the vacuum permittivity [ε0-constant](#ε0-constant) to the
elementary charge [qe-constant](#qe-constant): `ε0q=ε0/qe`. Since it
depends on the vacuum permittivity `ε0`, it is calculated as:
```rpl
    12 SIG  ε0q='CONVERT(Ⓒε0/Ⓒqe;1_F/(m*C))'
```
Its relative uncertainty is the same as the vacuum permittivity: 
```rpl
    12 SIG  Usε0q='→NUM(CONVERT(ROUND(UBASE(ⒸUrε0q*Ⓒε0q);-2);1_F/(m*C)))'  Urε0q='ⒸUrε0'
```

### qε0 constant

Product of the vaccum permittivity [ε0-constant](#ε0-constant) by the
elementary charge [qe-constant](#qe-constant): `qε0=ε0·qe`. Since it 
depends on the vacuum permittivity `ε0`, it is calculated as:
```rpl
    12 SIG  qε0='CONVERT(Ⓒqe*Ⓒε0;1_F*C/m)'
```
Its relative uncertainty is the same as the vacuum permittivity: 
```rpl
    12 SIG  Usqε0='→NUM(CONVERT(ROUND(UBASE(ⒸUrqε0*Ⓒqε0);-2);1_F*C/m))'  Urqε0='ⒸUrε0'
```

### ke constant

Coulomb constant as it appears in the expression of the Coulomb
force: `Fe=ke·q1·q2/r^2`. Since it depends on the vacuum permittivity 
`ε0`, it is calculated as:
```rpl
    12 SIG  ke='CONVERT(1/(4*Ⓒπ*Ⓒε0);1_(N*(m/C)^2))'
```
Its relative uncertainty is the same as the vacuum permittivity: 
```rpl
    12 SIG  Uske='→NUM(CONVERT(ROUND(UBASE(ⒸUrke*Ⓒke);-2);1_(N*(m/C)^2)))'  Urke='ⒸUrε0'
```

### εsi constant

Dielectric constant of silicon. The dielectric constant (or relative
permittivity), is a material property that measures how well an applied electric
field can penetrate a dielectric medium compared to a vacuum. A higher value
corresponds to a lower penetration. Its exact value is given by convention as:
```rpl
    12 SIG  εsi=11.9  Usεsi=0  Urεsi=0
```

### εox constant

SiO2 dielectric constant. The dielectric constant (or relative
permittivity) is a material property that measures how well an applied electric
field can penetrate a dielectric medium compared to a vacuum. A higher value
corresponds to a lower penetration. Its exact value is given by convention as:
```rpl
    12 SIG  εox=3.9  Usεox=0  Urεox=0
```

### I0 constant

Reference sound intensity. Sound intensity level or acoustic intensity level is
the level of the intensity of a sound relative to a reference value. It is a
logarithmic quantity, most often expressed in decibels dB. I0 is the sound
intensity used as a reference, corresponding to `β=0_dB` due to the definition 
of the sound pressure level `β=10·LOG10(I/I0)` where `I=I0`. Its exact value  
is given by convention as:
```rpl
    12 SIG  I0=0.000000000001_W/m^2  UsI0=0_W/m^2  UrI0=0
```

### Z0 constant

Vacuum characteristic impedance constant. It is a physical constant 
relating the magnitudes of the electric `E` and magnetic `H` fields 
of electromagnetic radiation travelling through free space. With
`Zo=|E|/|H|`. Since it depends on the vacuum permittivity `μ0`, it 
is calculated by:
```rpl
    12 SIG  Z0='CONVERT(Ⓒμ0*Ⓒc;1_Ω)'
```
Its relative uncertainty is the same as the one of the vacuum 
permittivity `μ0`: 
```rpl
    12 SIG  UsZ0='→NUM(CONVERT(ROUND(UBASE(ⒸUrZ0*ⒸZ0);-2);1_Ω))'  UrZ0='ⒸUrμ0'
```

### mD constant

Mass of the neutral deuterium atom which is a stable isotope of hydrogen. 
The electrically neutral deuterium atom H-2 (also known as heavy hydrogen) 
contains a single positively charged proton and a neutron in the nucleus,
and a single negatively charged electron bound to the nucleus by the 
Coulomb force. It accounts for about 0.0156% by number (0.0312% by mass) 
of all hydrogen in the ocean. Precise measurements of deuterium is obtained 
by spectrometry. Its measured value is:
```rpl
    12 SIG  mD=2.01410177812_u",           
```
with absolute and relative uncertainties:
```rpl
    12 SIG  UsmD=0.00000000012_u  UrmD='→NUM(ROUND(UBASE(ⒸUsmD/ⒸmD);-2))'
```

### mT constant

Mass of the neutral tritium atom which is an unstable isotope of hydrogen. 
The electrically neutral tritium atom H-3 is rare and radioactive with a 
half-life of ~12.3 years, Its nucleus contains a single positively charged
proton and two neutrons, surrounded by a single negatively charged electron
bound to the nucleus by the Coulomb force. Trace amounts are found in the 
atmosphere due to interaction of its gases with cosmic rays. Precise 
measurements of tritium mass is obtained by spectrometry. Its measured 
value is:
```rpl
    12 SIG  mT=3.0160492779_u"                       
```
with absolute and relative uncertainties:
```rpl
    12 SIG  UsmT=0.0000000024_u  UrmT='→NUM(ROUND(UBASE(ⒸUsmT/ⒸmT);-2))'
```

### mHe constant

Mass of the neutral helium atom. The electrically neutral helium atom 
He-4 contains two positively charged protons and two neutrons, and two
negatively charged electrons bound to the nucleus by the Coulomb force. 
Atomic helium constitutes about 24% of the total elemental mass of the 
Universe. Precise measurements of helium mass is obtained by spectrometry. 
Its measured value is: 
```rpl
    12 SIG  mHe=4.00260325413_u
```
with absolute and relative uncertainties:
```rpl
    12 SIG  UsmHe=0.00000000006_u  UrmHe='→NUM(ROUND(UBASE(ⒸUsmHe/ⒸmHe);-2))'
```

### mμ constant

Mass of the muon which is an elementary particle similar to the 
electron, with an electric charge of `−qe` and spin -1/2, but with 
a much greater mass. Classified as a lepton and it is an unstable
subatomic particle with a lifetime of 2.2 μs. Precise measurements
of the muon mass is deduced from energy conservation budget in pair 
creation reaction. Its measured value is: 
```rpl
    12 SIG  mμ=0.1134289257_u
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usmμ=0.0000000025_u  Urmμ='→NUM(ROUND(UBASE(ⒸUsmμ/Ⓒmμ);-2))'4.00260325413_u
```

### mτ constant

Mass of the tau which is an elementary particle similar to the 
electron, with an electric charge of `−qe` and spin -1/2, but it is
heaviest of leptons. It is an unstable subatomic particle with a 
lifetime of 2.9E-13 s. Precise measurements of the tau mass is 
deduced from energy conservation budget in pair creation reaction.
Its measured value is: 
```rpl
    12 SIG  mτ=1.90754_u
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usmτ=0.00013_u  Urmτ='→NUM(ROUND(UBASE(ⒸUsmμ/Ⓒmμ);-2))'
```

### G0 constant

Conductance quantum constant. It is the quantized unit of electrical 
conductance. It is required when measuring the conductance of a quantum 
point contact, and also, it appears explicitly in the Landauer formula: 
`G(μ)=G0·ΣTn(μ) over n` which relates the electrical conductance of a 
quantum conductor to its quantum properties. It is exactly calculated 
by:
```rpl
    12 SIG  G0='CONVERT(Ⓒqe^2/(Ⓒπ*Ⓒℏ);1_S)'  UsG0=0_S  UrkG0=0
```

### Rk constant

Von Klitzing constant. It appears in the expression of the Hall 
resistance `Rxy=Rk/ν` (`ν` being either an integer or a fraction) 
of the quantum Hall effect, a quantized version of the Hall effect 
which is observed in two-dimensional electron systems subjected to 
low temperatures and strong magnetic fields. It is exactly calculated 
by:
```rpl
    12 SIG  Rk='CONVERT(2*Ⓒπ*Ⓒℏ/Ⓒqe^2;1_Ω)'  UsRk=0_Ω  UrRk=0
```

### KJ constant

Josephson constant. The Josephson constant is a constant of 
proportionality that relates the potential difference across a 
Josephson junction to the frequency of irradiation. It's also 
the inverse of the magnetic flux quantum `ø`. It is exactly 
calculated by:
```rpl
    12 SIG  KJ='CONVERT(2*Ⓒqe/Ⓒh;1_Hz/V)'  UsKJ=0_Hz/V  UrKJ=0
```

### re constant

The classical electron radius. Through the Bohr radius `a0`, it 
depends on fine structure constant `α` and is calculated by:
```rpl
    12 SIG  re='CONVERT(Ⓒα^2*Ⓒa0;1_m)'
```
Through its dependance to `a0` and `α`, its relative uncertainty is three 
times the one of the fine structure constant: 
```rpl
    12 SIG  Usre='→NUM(CONVERT(ROUND(UBASE(ⒸUrre*Ⓒre);-2);1_m))'  Urre='→NUM(ROUND(3*Usα/α;-2))'
```

### rp constant

Proton charge radius. A direct measure of the proton radius. Since 2010,
the measure was done using either spectroscopy method with muonic hydrogen, 
and then with deuterium atom, or either using a more recent electron-proton 
scattering experiment. Its measured value is: 
```rpl
    12 SIG  mrp=8.4075-16_m
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usrp=0.0064-16_m  Urrp='→NUM(ROUND(UBASE(ABS(ⒸUsrp/Ⓒrp));-2))'
```

### σe constant

The Thomson cross-section. This type of scattering is valid when the field
energy `h·ν` is much less than the rest mass of the electron `m0·c^2`, the 
electric field of the incident wave accelerates the charged target-particle, 
causing it, in turn, to emit radiation at the same frequency `ν` as the 
incident wave, hence the scattering of the wave. This explains why Thomson 
scattering is important in plasma physics. Through the classical electron 
radius `re`, it depends on fine structure constant `α` and is calculated by:
```rpl
    12 SIG  σe='CONVERT(8*Ⓒπ*Ⓒre^2/3;1_m^2)'
```
Its relative uncertainty is six times the one of the fine structure constant: 
```rpl
    12 SIG  Usσe='→NUM(CONVERT(ROUND(UBASE(ⒸUrσe*Ⓒσe);-2);1_m^2))'  Urσe='→NUM(ROUND(6*ⒸUsα/Ⓒα;-2))'
```

### μe constant

Electron magnetic moment. The electron magnetic moment, or more 
specifically the electron magnetic dipole moment, is the magnetic 
moment of an electron resulting from its intrinsic properties of spin 
and electric charge. Its angular momentum comes from two types of 
rotation: spin and orbital motion. Therefore an external magnetic field
exerts a torque on the electron magnetic moment revealing its existence. 
Its measured value is: 
```rpl
    12 SIG  μe=-9.2847646917E-24_J/T,
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usμe=0.0000000029E-24_J/T  Urμe='→NUM(ROUND(UBASE(ABS(ⒸUsμe/Ⓒμe));-2))'
```

### μp constant

Proton magnetic moment. It is the magnetic dipole moment of the proton 
resulting from its intrinsic properties of spin and electric charge. Its 
angular momentum comes from two types of rotation: spin and orbital motion. 
Therefore an external magnetic field exerts a torque on the proton magnetic 
moment revealing its existence. Its measured value is: 
```rpl
    12 SIG  μp=1.41060679545E-26_J/T
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usμp=0.00000000060E-26_J/T  Urμp='→NUM(ROUND(UBASE(ABS(ⒸUsμp/Ⓒμp));-2))'
```

### μn constant

Neutron magnetic moment. It is the magnetic dipole moment of the meutron 
resulting from its intrinsic properties of spin. Normally it sould be 
zero for an elementary neutral particle because of zero charge. The fact
that it was non-vanishing prooves that the neutron is a composite particle. 
Its angular momentum comes from two types of rotation: spin and orbital 
motion. Therefore an external magnetic field exerts a torque on the 
neutron magnetic moment revealing its existence. Its measured value is: 
```rpl
    12 SIG  μn=-9.6623653E-27_J/T
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usμn=0.0000023E-27_J/T  Urμn='→NUM(ROUND(UBASE(ABS(ⒸUsμn/Ⓒμn));-2))'
```

### μμ constant

Muon magnetic moment. It is the magnetic dipole moment of the meutron 
resulting from its intrinsic properties of spin and electric charge. 
Its angular momentum comes from two types of rotation: spin and orbital 
motion. Therefore an external magnetic field exerts a torque on the muon 
magnetic moment revealing its existence. Its measured value is: 
```rpl
    12 SIG  μμ=-4.49044830E-26_J/T
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usμμ=0.00000010E-26_J/T  Urμμ='→NUM(ROUND(UBASE(ABS(ⒸUsμμ/Ⓒμμ));-2))'
```

### ge constant

Electron g-factor. It is a dimensionless quantity that characterizes 
the magnetic moment and angular momentum of an electron. It is the 
ratio of the magnetic moment (or, equivalently, the gyromagnetic ratio) 
of the electron to that expected of a classical particle of the same 
charge and angular momentum. The electron g-factor is one of the most 
precisely measured values in physics, which is: 
```rpl
    12 SIG  ge=-2.00231930436092
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usge=0.00000000000036  Urge='→NUM(ROUND(UBASE(ABS(ⒸUsge/Ⓒge));-2))'
```

### Mpl constant

Planck mass unit. As an attempt to devise a universal and natural units
system, the Planck units are combinations of basic universal constants. 
It can be viewed as the mass of a black hole with a Swarzhchild radius
of 2 Planck lengths (`rs=2·Lpl`) or, the minimum mass of a black hole 
is one half of the Planck mass, the latter having a Planck Length radius. 
Its value depends on the measured value of the gravitational constant 
`G` and is calculated by:
```rpl
    12 SIG  Mpl='CONVERT(√(Ⓒℏ*Ⓒc/ⒸG);1_kg)'
```
Its relative uncertainty is half the one of the gravitational constant: 
```rpl
    12 SIG  UsMpl='→NUM(CONVERT(ROUND(UBASE(ⒸUrMpl*ⒸMpl);-2);1_kg))'  UrMpl='ⒸUrG/2'
```

### Epl constant

Planck energy unit. As an attempt to devise a universal and natural units
system, the Planck units are combinations of basic universal constants. 
According to the mass-energy equivalence: `Epl=Mpl·c^2` it is the energy 
equivalent to the Planck mass. Considered to be the smallest possible unit 
of energy, which is theoretically meaningful within the framework of quantum 
gravity, where both effects of quantum mechanics and general relativity 
become significant; and also the energy scale at which the universe is thought 
to have existed at times near the start of the Big Bang, characterized by 
extremely high densities and temperatures. Its value depends on the measured 
value of the gravitational constant `G` and is calculated by:
```rpl
    12 SIG  Epl='→NUM(ROUND(CONVERT(√(Ⓒℏ*Ⓒc^5/ⒸG);1_GeV)'
```
Its relative uncertainty is half the one of the gravitational constant: 
```rpl
    12 SIG  UsEpl='→NUM(ROUND(CONVERT(ROUND(UBASE(UrEpl*Epl);-2);1_GeV);-2))'  UrEpl='ⒸUrG/2'
```

### T°pl constant

Planck temperature unit. As an attempt to devise a universal and natural 
units system, the Planck units are combinations of basic universal constants. 
It is the highest temperature that conventional physics can describe. It's a 
fundamental limit of quantum mechanics and is considered the temperature of 
the universe during the Big Bang when quantum gravity effects became
dominant. Its value depends on the measured value of the gravitational 
constant `G` and is calculated by: 
```rpl
    12 SIG  T°pl='CONVERT(√((Ⓒℏ*Ⓒc^5/ⒸG))/Ⓒk;1_K)'
```
Its relative uncertainty is half the one of the gravitational constant: 
```rpl
    12 SIG  UsT°pl='→NUM(CONVERT(ROUND(UBASE(ⒸUrT°pl*ⒸT°pl);-2);1_K))'  UrT°pl='ⒸUrG/2'
```

### Lpl constant

Planck length unit. As an attempt to devise a universal and natural units 
system, the Planck units are combinations of basic universal constants. It 
is the smallest distance that can be measured, and it represents the 
scale at which quantum gravity effects become dominant. It is the distance
travelled by light during one Planck time `Tpl`. Its value depends on the 
measured value of the gravitational constant `G` and is calculated by:
```rpl
    12 SIG  Lpl='CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^3);1_m)'
```
Its relative uncertainty is half the one of the gravitational constant: 
```rpl
    12 SIG  UsLpl='→NUM(CONVERT(ROUND(UBASE(ⒸUrLpl*ⒸLpl);-2);1_m))'  UrLpl='ⒸUrG/2'
```

### Tpl constant

Planck time unit. As an attempt to devise a universal and natural units 
system, the Planck units are combinations of basic universal constants. 
It is the shortest time interval that can be measured and it is fundamental
in the study of the universe beginning. It is the time required for light
to travel one Planck length `Lpl`. Its value depends on the measured value 
of the gravitational constant `G` and is calculated by:
```rpl
    12 SIG  Tpl='CONVERT(√(Ⓒℏ*ⒸG/Ⓒc^5);1_s)'
```
Its relative uncertainty is half the one of the gravitational constant: 
```rpl
    12 SIG  UsTpl='→NUM(CONVERT(ROUND(UBASE(ⒸUrTpl*ⒸTpl);-2);1_s))'  UrTpl='ⒸUrG/2'
```

### Eh constant

Hartree energy constant. It is a unit of energy used in atomic physics 
and computational chemistry, which is also used in molecular orbital 
calculations. It is approximately the negative electric potential energy 
of an electron in a hydrogen atom's ground state, and also approximately 
twice the ionization energy of a hydrogen atom. Its value depends on the 
measured value of the Rydberg constant `R∞` and is calculated by:
```rpl
    12 SIG  Eh='CONVERT(2*Ⓒh*Ⓒc*ⒸR∞;1_J)'
```
Its relative uncertainty is the same as the one of the Rydberg constant: 
```rpl
    12 SIG  UsEh='→NUM(CONVERT(ROUND(UBASE(ⒸUrEh*ⒸEh);-2);1_J))'  UrEh='ⒸUrR∞'
```

### γe constant

Electron gyromagnetic ratio. It is the ratio of the electron's magnetic 
moment to its angular momentum. It can be used to determine the direction 
of precession and the resonance frequency of an electron in a magnetic 
field. Its value depends on the electron magnetic moment `μe` and is 
calculated by: 
```rpl
    12 SIG  γe='CONVERT(2*ABS(Ⓒμe)/Ⓒℏ;1_(s*T)^-1)'
```
its relative uncertainty is the same as the electron magnetic moment
'μe':
```rpl
    12 SIG  Usγe='→NUM(CONVERT(ROUND(UBASE(ⒸUrγe*Ⓒγe);-2);1_(s*T)^-1))'  Urγe='ⒸUrμe'
```

### γp constant

Proton gyromagnetic ratio. It is the ratio of the proton's magnetic 
moment to its angular momentum. It can be used to determine the direction 
of precession and the resonance frequency of a proton in a magnetic field. 
The proton gyromagnetic ratio is used in nuclear magnetic resonance (NMR) 
imaging, such as in MRI scans.  Its value depends on the proton magnetic 
moment `μp` and is calculated by: 
```rpl
    12 SIG  γp='CONVERT(2*ABS(Ⓒμp)/Ⓒℏ;1_(s*T)^-1)'
```
its relative uncertainty is the same as the proton magnetic moment
'μp':
```rpl
    12 SIG  Usγp='→NUM(CONVERT(ROUND(UBASE(ⒸUrγp*Ⓒγp);-2);1_(s*T)^-1))'  Urγp='ⒸUrμp'
```

### γn constant

Neutron gyromagnetic ratio. It is the ratio of the Neutron's magnetic 
moment to its angular momentum. It is a characteristic of the neutron's 
nuclear spin and its sign determines the direction of precession. It is 
used in nuclear magnetic resonance (NMR) applications, such as MRI imaging. 
In NMR spectroscopy, it is used to measure detection sensitivity. Its 
value depends on the neutron magnetic moment `μn` and is calculated by: 
```rpl
    12 SIG  γn='CONVERT(2*ABS(Ⓒμn)/Ⓒℏ;1_(s*T)^-1)'
```
its relative uncertainty is the same as the neutron magnetic moment
'μn':
```rpl
    12 SIG  Usγn='→NUM(CONVERT(ROUND(UBASE(ⒸUrγn*Ⓒγn);-2);1_(s*T)^-1))'  Urγn='ⒸUrμn'
```

### θw constant

The weak mixing angle or Weinberg angle. It is a parameter in the 
Weinberg–Salam theory of the electroweak interaction, part of the 
Standard Model of particle physics. It is the angle by which spontaneous 
symmetry breaking rotates the original W0 and B0 vector boson plane, 
producing as a result the Z0 boson, and the photon. From the mesurement 
of the following dimensionless parameter (for the W and Z bosons): 
`(sinθw)^2 = 1 - (mW/mZ)^2 = 0.22305 ± 0.00023` 
the value of the angle `θw` is then calculated as: 
```rpl
    5 SIG  θw='CONVERT(ASIN(√(0.22305));1_°)'
```
With the following uncertainties:
```rpl
    5 SIG  Usθw='→NUM(CONVERT(ROUND(ASIN(√(0.22305+0.00023))-ASIN(√(0.22305-0.00023));-2);1_°))'
           Urθw='→NUM(ROUND(UBASE(ABS(ⒸUsθw/CONVERT(ASIN(√(0.22305));1_°)));-2))'
```

### ΔfCs constant

Caesium (Cs) hyperfine transition. It is the transition between the two 
hyperfine ground states of the caesium atom. The frequency `ΔfCs` of this 
transition is used to define the second as the official time unit of the 
International System of Units (SI). Its exact value (without uncertainty) 
is given by convention as: 
```rpl
    12 SIG  ΔfCs=9192631770_Hz  UsΔfCs=0_Hz  UrΔfCs=0
```


## Dates Constants

### BastilleDay constant

French national day

### MartinLutherKingDeath constant

Martin Luther King Jr., an African-American clergyman and civil rights movement
leader, was fatally shot at the Lorraine Motel in Memphis, Tennessee, on April
4, 1968, at 6:01 p.m.

### IndependenceDay constant

Independence Day, known colloquially as the Fourth of July, is a federal holiday
in the United States which commemorates the ratification of the Declaration of
Independence by the Second Continental Congress on July 4, 1776, establishing
the United States of America.

## Computing constants

### No constant

The value of "No" is false

### Yes constant

The value of "Yes" is true

### UnixEpoch constant

Date from which all dates are counted in Unix computer systems.

### SinclairZX81RAM constant

Amount of memory in the Sinclair ZX81 personal computer.
This is also the amount of memory used in the video memory for the mouse cursor
at its smallest size on modern computers.

### PageSize constant

Base page size in the virtual memory system of most modern computers. Note that
for performance reason, many modern computers now support multiple page sizes.

### HelloWorld constant

A constant often used in computer programs to denote joy, happiness and the
emergence into the world of some new computer language.
