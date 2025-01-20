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

## Chemistry constant

### NA constant

Avogradro constant is the number of constituent particles per mole. Since 
the redefinition of the mole in 2019, as being the amount of substance 
containing exactly `NA` particles, the `NA` constant is therefore defined 
as the following exact value:
```rpl
    12 SIG  NA=6.02214 076E23_mol⁻¹  UsNA=0_mol⁻¹  UrNa=0
```

### k constant

The Boltzmann constant is the proportionality factor that relates the average
relative thermal energy of particles in a gas with the thermodynamic temperature
of the gas. 

Since the 2019 redefinition of the SI units, the `k` constant is exactly defined 
as the following exact value:
```rpl
    12 SIG  k=1.38064 9E-23_J/K  Usk=0_J/K  Urk=0
```

### Vm constant

Molar volume of an ideal gas at 1 atmosphere of pressure and 0°C. By convention, 
it is chosen as the result of the following exact calculation:
```rpl
    12 SIG Vm='CONVERT(ⒸR*ⒸStdT/ⒸStdP;1_m^3/mol)'  UsVm=0_m^3/mol  UrVm=0
```

### R constant

The universal gas constant is the molar equivalent to the Boltzmann constant,
expressed in units of energy per temperature increment per amount of substance,
rather than energy per temperature increment per particle. 

Since the 2019 redefinition of the SI units, it is chosen as the result of 
the following exact calculation:
```rpl
    12 SIG R='CONVERT(ⒸNA*Ⓒk;1_J/(mol*K))'  UsR=0_J/(mol*K)  UrR=0
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
    12 SIG StdT=273.15_K  UsStdT=0_K  UrStdT=0
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
    12 SIG StdP=101.325_kPa  UsStdP=0_kPa  UrStdP=0
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
    12 SIG σ='CONVERT(Ⓒπ^2/60*Ⓒk^4/(Ⓒℏ^3*Ⓒc^2);1_W/(m^2*K^4))'  Usσ=0_J/(mol*K)  Urσ=0
```

#### Mu Molar Mass Constant

The molar mass constant is a physical constant defined as one twelfth of 
the molar mass of carbon-12: `MC12/12`. The molar mass of an element or 
compound is its relative atomic mass (atomic weight) or relative molecular 
mass multiplied by the molar mass constant. Since 2019, the SI definition 
of mole changed such that the molar mass constant remains nearly but no 
longer exactly 1 g/mol. For internal consistency this value depends
therefore on the mass unit `u` and is calculated by the following expression:
```rpl
    12 SIG Mu='CONVERT(ⒸNA*Ⓒu;1_kg/mol)'
```
Due to its expression, the relative uncertainty of the molar mass is 
the same as the one of the mass unit `u`:
```rpl
    12 SIG  UsMu='→NUM(CONVERT(ROUND(UBASE(ⒸUrMu*ⒸMu);-2);1_kg/mol))'  UrMu='ⒸUru'
```

#### MC12 Molar Mass of carbon-12

Since 1960, mole is the amount of substance of a system which contains
as many elementary entities as there are atoms in 12 gram of carbon-12. 
Since 2019, the SI definition of mole changed such that the molar mass of 
carbone-12 remains nearly but no longer exactly 12 g/mol. For internal 
consistency this value depends therefore on the mass unit `u` and is 
calculated by the following expression:
```rpl
    12 SIG MC12='CONVERT(12*ⒸMu;1_kg/mol)'
```
Due to its expression, the relative uncertainty of the molar mass is 
the same as the one of the molar mass `Mu`:
```rpl
    12 SIG  UsMC12='→NUM(CONVERT(ROUND(UBASE(ⒸUrMC12*ⒸMC12);-2);1_kg/mol))'  UrMC12='ⒸUrMu'
```

#### Are Electron relative atomic mass

The experimental value of the electron relative atomic mass is an 
important constant (usually noted as `Ar(e))`) which is needed to 
calculate the unit mass value `u`. According to COTATA2022, it is 
a dimensionless value measured as:
```rpl
    12 SIG Are=5.485799090441E-4
```
Its absolute and relative uncertainties come from measurement:
```rpl
    12 SIG  UsAre=0.000000000097E-4  UrAre='→NUM(ROUND(UBASE(ⒸUsAre/ⒸAre);-2))'
```

#### n0 Loschmidt constant

The Loschmidt constant or Loschmidt's number is the number of particles
(atoms or molecules) of an ideal gas per volume (the number density), 
and usually quoted at standard temperature and pressure. Since 2019 with 
the redifinition of the mole, it is calculated exactly as:
```rpl
    12 SIG n0='CONVERT(ⒸNA/ⒸVm;1_m^-3)'  Usn0=0_m^-3  Urn0=0
```

#### SoR Sakur-Tetrode constant

The Sakur-Tetrode constant gives the absolute entropy at an absolute 
temperature of `T=1_K` and standard atmospheric pressure `StdP=101.325_kPa` 
for one mole of an ideal monoatomic gas composed of particles of mass equal 
to the atomic mass constant. It is used in the general expression of the 
entropy of an ideal monoatomic gas of relative atomic mass `Ar` at a pressure 
`p_kPa` and a temperature `T_K`, which is given by:
`S=ⒸR*(SoR+3/2*LN(Ar)−LN((p_kPa)/ⒸStdP)+5/2*LN((T_K)/(1_K)))`. 

The SoR constant is calculated from the measured value of the unit mass `u`.
```rpl
    12 SIG  SoR='5/2+LN(UBASE(u*Ⓒk*(1_K)/(2*Ⓒπ*Ⓒℏ^2))^1.5*Ⓒk*(1_K)/ⒸStdP)'
```
The uncertainties are evaluated as:
```rpl
    12 SIG  UsSoR='ROUND(UBASE(ⒸUrSoR*ABS(ⒸSoR));-2)'  UrSoR=4.0E-10
```


## Physics constants

### ⅉ constant

Notation often used in physics for the imaginary verifying ⅉ²=-1.

### c constant

Speed of light in vaccuum, a universal physical constant that is exactly equal
to 299 792 458 metres per second (by definition of the metre).

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
    12 SIG  c=299 792 458_m/s  Usc=0_m/s  Urc=0
```


### ε0 constant

Vacuum permittivity, commonly denoted `ε0` (pronounced "epsilon nought" or
"epsilon zero"), is the value of the absolute dielectric permittivity of
classical vacuum. It may also be referred to as the permittivity of free space,
the electric constant, or the distributed capacitance of the vacuum. It is an
ideal (baseline) physical constant. It is a measure of how dense of an electric
field is "permitted" to form in response to electric charges and relates the
units for electric charge to mechanical quantities such as length and force.

Its value depends on the vaccuum permeability constant and is calculated with:
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

Its value depends on the fine structure constant measurement and is calculated with:
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
    12 SIG  h=6.62607 015E-34_J*s  Ush=0_J*s  Urh=0  
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
as `qe=1.60217 6634⁳⁻¹⁹` coulombs. Since the 2019 redefinition of SI base units,
the seven SI base units are defined by seven fundamental physical constants, of
which the elementary charge is one. As a consequence of this change, the value
of that constant in DB48X differs from the value in the HP50G, which named it 
`q`, with value `1.60217 733⁳⁻¹⁹` coulombs. 

The exact value of `qe` is then:
```rpl
    12 SIG  qe=1.602176634E-19_C  Usqe=0_C  Urqe=0  
```

### me constant

Electron mass. In particle physics, the electron mass is the mass of a
stationary electron, also known as the invariant mass of the electron. It is one
of the fundamental constants of physics. Its value is closely related to the unit
mass measurement through the electron relative atomic mass: `me=u*Ar(e)` where 
`Ar(e)` is determined iteratively by frequency measurements using Penning trap. 

According to https://en.wikipedia.org/wiki/Electron_mass this is equivalent to
determine its experimental value by the following calculation where it depends 
on the fine structure constant `α` and the Rysberg constant `R∞`:
```rpl
    12 SIG  me=CONVERT(2*Ⓒh*ⒸR∞/((Ⓒα^2)*Ⓒc);1_kg)
```
Its relative uncertainty is therefore on the same order either of the sum 
`2*Urα+UrR∞` or is the same as the relative uncertainty of the unit mass:
```rpl
    12 SIG  Usme='ⒸUru'
```

### mn constant

Neutron mass. Being electrically neutral, the neutron emission, wrongly 
interpreted at the beginning as some sort of γ radiation was correctly 
identified in 1932 by Chadwick as the neutron hypothesized by Rutherford
as a neutral particule having about the same mass as the proton. 

The value of its mass is determined experimentally by mass spectrometry as:
```rpl
    12 SIG  mn=1.67492750056E-27_kg
```
with absolute and relative uncertainties:
```rpl
    12 SIG  Usmn=0.00000000085E-27_kg  Urmn='→NUM(ROUND(UBASE(ⒸUsmn/Ⓒmn);-2))'
```

### mp constant

Mass of the proton. Being the hydrogen nucleus, discovered and named 
by Ernest Rutherford, the proton was considered at first to be an 
elementary particle. It is now recognized as being composed of two 
valence up quarks of charge `+⁠2/3e` each, and one valence down quark
of charge `−⁠1/3⁠e`. The rest masses of quarks contribute only about 1% 
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

### mH constant

Mass of the neutral hydrogen atom. The electrically neutral hydrogen
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

### mD constant

Mass of the neutral hydrogen atom. The electrically neutral hydrogen
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

### qme constant

Ratio between the electron charge and its mass.

### mpme constant

Ratio between the mass of the proton and the mass of the electron.

### α constant

Fine-structure constant. In physics, the fine-structure constant, also known as
the Sommerfeld constant, commonly denoted by α (the Greek letter alpha), is a
fundamental physical constant which quantifies the strength of the
electromagnetic interaction between elementary charged particles.

It is a dimensionless quantity, independent of the system of units used, which
is related to the strength of the coupling of an elementary charge e with the
electromagnetic field.

### ø constant

Magnetic flux quantum. The (superconducting) magnetic flux quantum is a
combination of fundamental physical constants: the Planck constant h and the
electron charge e. Its value is, therefore, the same for any superconductor.

### F constant

Faraday constant. In physical chemistry, the Faraday constant is a physical
constant defined as the quotient of the total electric charge (q) by the amount
(n) of elementary charge carriers in any given sample of matter

### R∞ constant

Rydberg constant. In spectroscopy, the Rydberg constant is a physical constant
relating to the electromagnetic spectra of an atom. The constant first arose as
an empirical fitting parameter in the Rydberg formula for the hydrogen spectral
series, but Niels Bohr later showed that its value could be calculated from more
fundamental constants according to his model of the atom.

### a0 constant

Bohr radius. The Bohr radius is a physical constant, approximately equal to the
most probable distance between the nucleus and the electron in a hydrogen atom
in its ground state.

### μB constant

Bohr magneton. In atomic physics, the Bohr magneton is a physical constant and
the natural unit for expressing the magnetic moment of an electron caused by its
orbital or spin angular momentum. In SI units, the Bohr magneton is defined as
`μB=qe*ℏ/(2·me)`.


### μN constant

The nuclear magneton is a physical constant of magnetic moment, defined in SI
units by `μN=qe·ℏ/(2·mp)`.


### λ0 constant

Photon wavelength. Photon energy can be expressed using any unit of energy such
as the electronvolt (eV) or the Joule (J). For short wavelength sources,
researchers often discuss photon energies in units of eV (or keV for hard
X-rays) out of convenience.  The SI definition for 1 eV derives from the
[definitional value of the electron charge](#me-constant). Photon energy `E` in
eV can be computed from wavelength `l` in nm as: `E=λ0/l`.

### f0 constant

Photon frequency. This is the frequency associated to the
[photon wavelength λ0][#λ0-constant].

### λc constant

Compton wavelength. The Compton wavelength is a quantum mechanical property of a
particle, defined as the wavelength of a photon whose energy is the same as the
rest energy of that particle (based on the mass–energy equivalence).

The standard Compton wavelength `λ` of a particle of mass `m` is given by
`λ=h/(m·c)`

### rad constant

One radian.

### twoπ constant

Two π radian (one full circle).

### angl constant

Half turn in degrees.

### c3 constant

Wien's constant. In physics, Wien's displacement law states that the black-body
radiation curve for different temperatures will peak at different wavelengths
that are inversely proportional to the temperature. The shift of that peak is a
direct consequence of the Planck radiation law, which describes the spectral
brightness or intensity of black-body radiation as a function of wavelength at
any given temperature. However, it had been discovered by German physicist
Wilhelm Wien several years before Max Planck developed that more general
equation, and describes the entire shift of the spectrum of black-body radiation
toward shorter wavelengths as temperature increases.

Formally, the wavelength version of Wien's displacement law states that the
spectral radiance of black-body radiation per unit wavelength, peaks at the
wavelength `λpeak=c3/T` where `T` is absolute temperature.

### kq constant

Ratio of the [Boltzman constant](#k-constant) by the
[elementary charge](#qe-constant): `kq=k/qe`.

### ε0q constant

Ratio of the [vaccum permittivity](#ε0-constant) by the
[elementary charge](#qe-constant): `ε0q=ε0/qe`.


### qε0 constant

Product of the [vaccum permittivity](#ε0-constant) by the
[elementary charge](#qe-constant): `qε0=ε0·qe`.

### εsi constant

Dielectric constant of silicon. The dielectric constant (or relative
permittivity), is a material property that measures how well an applied electric
field can penetrate a dielectric medium compared to a vacuum. A higher value
corresponds to a lower penetration.

### εox constant

SiO2 dielectric constant. The dielectric constant (or relative
permittivity), is a material property that measures how well an applied electric
field can penetrate a dielectric medium compared to a vacuum. A higher value
corresponds to a lower penetration.

### I0 constant

Reference sound intensity. Sound intensity level or acoustic intensity level is
the level of the intensity of a sound relative to a reference value. It is a
logarithmic quantity, most often expressed in decibels dB. I0 is the sound
intensity used as a reference, corresponding to 0dB.


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
