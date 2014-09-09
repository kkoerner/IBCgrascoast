//--documentation Lina' paper version-------------------------------------------------------------------------
/**
 * \page ODDox About this documentation
 * \htmlonly
<h1> <small> <small> <small> <small>
Created by<br>
<br>
</small>
Lina Weiss and Katrin Koerner<br>
<small>
Department of Vegetation Ecology and Nature Conservation<br>
Institute of Biology and Biochemistry, Potsdam University<br>
Maulbeerallee 2 <br>
GER-14469 Potsdam<br>
Germany<br>
<br>
October 2013, last updated 6th May 2014
</small>
</small>
</small>
</small>
</h1>
\endhtmlonly
<HR>
<b>About this documentation</b><br>
<br>
The ODdox documentation (<a href="#ref10a">Topping et al, 2010</a>) is a hybrid between the ODD protocol (<a href="#ref4a"> Grimm et al. 2006</a>) for
describing IBMs and Doxygen ( http://www.doxygen.org ) a standard software tool designed to increase the accessibility of program code. In ODdox documentation
provides a set of html documents generated by Doxygen which form cross-linked documentation of all classes, methods and variables used in a progam code.
This allows the user to browse through classes and see the inter-relationships in the code. At the lowest level the code is presented together with a brief
description of what it does, and including the comments placed inside the code itself. The ODdox thus provides a powerful way of documentating IBMs and
allowing others to choose to see precisely what has been done or to get an overview quickly without the details. <br>
<br>
This version of the IBC_grass documentation corresponds to the model version used by Lina Weiss in <a href="#ref14">Weiss et al.(2014)</a>.
<br>
<HR>
<b>References:</b>
<br>
- <a name="ref4a">V. Grimm,U. Berger, F. Bastiansen, S. Eliassen, V. Ginot, J. Giske, J. Goss-Custard, T. Grand, S. K. Heinz, G. Huse, A. Huth, J. U. Jepsen, C. Joergensen, W. M. Mooij, B. Muller, G. Peer, C. Piou, S. F. Railsback, A. M. Robbins, M. M. Robbins, E Rossmanith, N. Ruger, E. Strand, S. Souissi, R. A. Stillman, R. Vaboe, U. Visser, D. L. DeAngelis (2006). A standard protocol for describing individual-based and agent-based models. <i>Ecological Modelling</i> 198: 115-126.<br>
- <a name="ref10a">C.J. Topping, T.T. Hoye and C.R. Olesen 2010. Opening the black box: Development, testing and documentation of a mechanistically rich agent-based model.   Ecological Modelling 221 (2), 245-255
doi:10.1016/j.ecolmodel.2009.09.014<br>
- <a name="ref14">Weiss L, H Pfestorf, F May, K K&ouml;rner, S Boch, M Fischer, J M&uuml;ller,
  D Prati, S Socher , F Jeltsch (2014) Grazing response patterns indicate isolation of semi-natural
  European grasslands. \e Oikos 123 (5) 599-612.<br>
<HR>
 *
 */
/**
\page ODDbase ODD-Description of the basic model
 *
 * \subpage ODDox About..
 * \todo cross-link ODD description below and add figures and tables
 *
 * \section sOver Overview
 * The model describes the development of a community of perennial plant functional types (PFTs).
 * It is based on the grassland model developed by May et al. (2009),
 * augmented by the incorporation of clonal plant types.
 * The model is individual-based and spatially explicit. It includes the entities seeds,
 * individual plants and grid cells. Seeds are characterized by their spatial position and
 * the state variables mass and age. Individual plants, which are either ramets in the case
 * of clonal plant types or non-clonal individuals, are described by their position, shoot,
 * root and reproductive mass, the number of ramets within a genet in case of clonal plant types
 * and the duration of resource stress exposure (Table S1); additionally, 22 parameters define the
 * plant&primes functional type (Table S2). The model simulates an area of approx. 3 square meters,
 * i.e. a grid of 173*173 cells representing 1 square cm each. Each cell can be occupied by the stemming
 * point of one individual plant only, but the individuals&prime zone of influence can cover more than one
 * grid cell (see below). A time step corresponds to one week, simulation time is 100 years with a
 * vegetation period of 30 weeks per year. Fig. 1 shows the scheduling of sub-models that are run for
 * each individual plant, which are briefly explained in the following:
 *
 * \section scomp Resource competition
 * Individual plants gain resources on their zone of influence (ZOI), a circular area around their
 * stemming location (Schwinning and Weiner 1998, Weiner et al. 2001). To independently represent
 * above- and belowground competition, plants have an above- and belowground ZOI (Fig. S1).
 * The ZOIs&prime radius depends on the mass of the corresponding plant compartment.
 * Thus, as plants grow their ZOIs grow and start to overlap with ZOIs of neighboring plants.
 * On the overlapping areas, plants compete for resources, i.e. resources are shared according to
 * relative competitive abilities.
 *
 * For above-ground competition, size-asymmetric competition is assumed, i.e. larger and higher plants
 * receive a higher share of resources in proportion to their shoot geometry traits (plant mass and
 * growth form), while belowground competition is assumed to be size-symmetric, i.e. resources are
 * partitioned equally between individuals (Schwinning and Weiner 1998, Weiner et al. 2001).
 *
 * Intra- versus interspecific competitive effects are included into the model by considering individuals
 * of the same PFT as con- and of different PFTs as hetero-specifics. The relative competitive ability of
 * a plant is higher the less con-specific individuals cover the same cell. Thereby intra-specific
 * competition is increased relatively to interspecific competition, implicating niche differentiation
 * of resource competition (Chesson 2000).
 *
 * \section sgrowth Plant growth
 * Plant growth depends on the resources an individual plant acquires in its ZOIs.
 * Resources are adaptively allocated to shoot and root growth in order to balance the uptake of the
 * limiting above- or belowground resources. Maximal resource allocation to growth is limited by a
 * PFT-specific maximal resource utilization rate. Growth of reproductive biomass takes place in
 * weeks 16 to 20 each year.
 * \sa CPlant::Grow2()
 *
 * \section sdisp Seed dispersal
 * Seeds of all plants are dispersed in week 21 each year. The number of seeds dispersed by an
 * individual plant is calculated by dividing its reproductive biomass by the type-specific mass of one
 * seed (Schippers et al. 2001, Lehsten and Kleyer 2007). Dispersal distance and direction are chosen
 * stochastically from a log-normal (distance) or respectively uniform (direction) distribution
 * (Stoyan and Wagner 2001).
 * \sa CGrid::DispersSeeds()
 *
 * \section smort Mortality
 * If resource uptake is below a fixed threshold fraction of the optimal resource uptake of a plant,
 * it suffers from resource stress. The probability of dying increases linearly with each week of
 * stress exposure. A PFT-specific maximal survival time under resource stress determines death of a plant.
 * Additionally, a constant background mortality of 0.7\% per week, corresponding to 20\% annual mortality
 * (Schippers et al. 2001), is assumed. At the end of each year, 50 \% of all seeds die randomly due to
 * winter mortality. Also, there is a winter dieback of 50\% of above-ground biomass of all plants.
 * \sa CPlant::Kill(), CPlant::WinterLoss(), CGrid::SeedMortWinter()
 *
 * \section sgraz Grazing
 * Grazing takes place randomly during the vegetation period with a fixed weekly probability.
 * Individual plants affected by grazing are chosen randomly, with a probability that depends on its
 * traits shoot size, growth form and a PFT-specific palatability. A grazing event corresponds to a
 * reduction of shoot mass of 50\% and the removal of its entire reproductive biomass. Plants are chosen
 * randomly until 50 \% of the above-ground biomass on the grid is removed.
 * \sa CGrid::Disturb(), CGrid::Grazing()
 *
 * \section sestab Establishment
 * Seeds germinate and establish during four weeks after dispersal (autumn) and four weeks in spring
 * of the next growing season. Only one seedling per cell is allowed to establish (with shoot and root
 * mass equal to seed mass). The respective seedling is the winner of a weighted lottery competition
 * between seedlings in one cell, with seed mass being the measure for competitive ability of the
 * seedlings (Chesson and Warner 1981, Schippers et al. 2001).
 * \sa CGrid::EstabLottery()
 *
 * \section sclon Clonal growth and resource sharing
 * Clonal plant growth is integrated via the ability of individuals to propagate vegetatively
 * (growth, dispersion and establishment of spacers) and a PFT-specific ability to share resources
 * between ramets of one genet. Here we assume that for each individual (i.e. ramet) one spacer can
 * grow at a time step. Analogously to generative reproduction, 5\% of acquired resources are allocated
 * to the growth of the spacer. The length of spacers is randomly chosen from a PFT-specific normal
 * distribution. Direction of spacer growth is chosen randomly from a uniform distribution.
 *
 * At the end of the vegetation period, all spacers of clonal individuals establish unless the cell
 * they have reached by that time is occupied by the center of a different individual. If this is the
 * case, the spacer is removed from the grid.
 *
 * Clonal plants of the integrator-type (sensu Oborny et al. 2000) share resources throughout the
 * whole genet, i.e. resources that are not essential for survival of the ramets are provided for
 * the genet and subsequently equally shared among ramets.
 * \sa CGrid::RametEstab(), CGrid::DispersRamets(), CGrid::DistribResource()
 *
 * \section sinit Initialization
 * Ten seedlings of each of the 86 PFT (see below) are randomly distributed over the grid.
 * Resources are spatially and temporally distributed homogenously in the simulations.
 * \sa CGridEnvir::InitRun()
 *
 * \section straits Trait syndromes and clonal attributes
 * Each plant in the model belongs to a certain plant functional type (PFT), which differ in their
 * attributes, or parameters, for four functional trait syndromes that are based on well-documented
 * trade-offs, and two clonal attributes (Tables S1 and S2):
 * \sa SPftTraits
 *
 * \subsection ssgf Growth form
 * The ratio between leaf mass and total shoot mass characterizes the growth form of a plant.
 * Low values indicate that plants invest more resources in building up support tissue instead of leaves.
 * Hence, the leaf/mass ratio reflects a trade-off between above-ground competitive ability (for light)
 * and the relative growth rate of a plant without competition.
 *
 * \subsection ssmaxplant Maximum plant mass
 * Maximum plant mass comprises attributes that reflect the competition colonization trade-off.
 * Maximum plant mass is positively correlated with individual seed mass (Eriksson and Jakobsson 1998),
 * and hence negatively correlated with seed number and dispersal distance. PFTs with low seed masses
 * produce more seeds per resources gained for reproduction (Leishman 2001, Schippers et al. 2001)
 * and these seeds disperse over longer distances (Jongejans and Schippers 1999). However, PFTs with
 * high seed masses experience higher recruitment success (see Establishment section).
 *
 * \subsection ssres Resource response
 * The trade-off between competitive ability and stress tolerance (Grime 2001) is integrated in the
 * trait syndrome \�resource response\�. A high relative growth rate in resource-rich environments comes
 * along with low stress resistance, i.e. a short period a plant can survive under resource stress.
 *
 * \subsection sspalat Grazing response
 * Response to grazing can follow two strategies: tolerance by fast re-growth or avoidance by a low
 * palatability (Bullock et al. 2001). We use a functional analogue to the specific leaf area (SLA)
 * to characterize the trade-off between high longevity, structural strength or high allocation to
 * defensive compounds (low SLA, low palatability) and fast growth through high efficiency of light
 * interception (high SLA, high palatability) (Westoby et al. 2002).
 *
 * \subsection ssclonality Clonal attributes
 * Clonal plants are characterized by the potential of sharing resources between ramets via persistent
 * connective tissues (Oborny et al. 2000) and the trade-off between number and length of spacers
 * between ramets (Winkler et al. 2002).
 *
 * \section spftdef Definition of plant functional types
 * Within the &apos;Biodiversity Exploratories&apos;, experimental plots of semi-natural grasslands are monitored
 * (Fischer et al. 2010). A total number of 197 perennial herbaceous plant species found on the plots,
 * used as pastures, were classified into PFTs via traits listed in the trait databases LEDA
 * (Kleyer et al. 2008), BiolFlor (Klotz et al. 2002) and Clo-Pla3 (Klimesova and De Bello, 2009).
 * Classification into PFTs was conducted by characterizing these species by the trait syndromes defined
 * in the model as follows (see Table 1 for trait parameters and a complete list of plant species and
 * respective traits in Supplementary Material):
 *
 * The Growth form of a species was defined as rosette, semi-rosette or erect by using the \�rosettes\�
 * attribute from the database BiolFlor. The Resource response strategy (competitor, intermediate or
 * stress tolerator type) of a species was classified following Grime (1974, 1979) by applying the
 * \�ecological strategy\� attribute from BiolFlor. Species\� Grazing response strategies (tolerator,
 * intermediate, and avoider) were also extracted from BiolFlor by using the attribute \�grazing tolerance\�
 * from the trait group \�grassland utilization indicator value\�. The Maximum plant size of a species
 * (large, medium, and small) was obtained from the trait \�seed release height\� of the LEDA-database
 * by classifying three equidistantly spaced levels. Seed release height was used rather than canopy
 * height, as it is more closely correlated to seed mass (REF), which is the relevant parameter
 * in the model. The clonal attributes Spacer length and integration were derived from the CloPla3 database.
 *
 * If more than one database entry was available for a species, we selected for completeness,
 * provenance (Central European records preferred) and habitat type. Furthermore clonal attributes were
 * selected for the clonal growth organ with maximal lateral spread per year. Clonal integration was
 * approved if connective tissues between mother and daughter ramets persist for two or more years while
 * species with connective tissues persisting less than two years are assumed not to share resources
 * within the individual. Spacer length was grouped via the \�lateral spread\� attribute of Clo-Pla3.
 * Short spacer length was assorted less than 0.01 meters per year, long spacer grow includes
 * Clo-Pla3-classes 0.01-0.25 meters a year and longer. This translation of species into PFT resulted
 * in a list of 86 PFTs that served as species pool for model initialization.
 *
 */
//--documentation Felix' paper version-------------------------------------------------------------------------
/* *
\page ODDbase ODD-Description of the basic model

The model description follows the ODD protocol (overview,
design concepts, details) for describing individualbased
models (Grimm et al. 2006).

\section Purpose
The model is designed to evaluate the response of plant
functional type (PFT) diversity towards grazing under
different local environmental conditions and differentiated
assumptions about plant-plant competition.

\section variables State variables and scales
The model includes the entities seeds, individual plants
and grid cells (Table 1). Seeds are described by the state
variables position, age, and mass. Plant individuals are
characterized by their position, the mass of three plant
compartments (shoot, root and reproductive mass), the
duration of resource stress exposure, and they are classified
as a certain PFT with specific trait attribute parameters
(Table 3). In a simplified model version only two plant
compartments, vegetative and reproductive mass, are taken
into consideration. Spatially, plants are described by their
�zone-of-influence� (ZOI), i.e. a circular area around
their location (Schwinning and Weiner 1998, Weiner et
al. 2001). Within this area the individual can acquire
resources and if the ZOIs of neighbouring plants overlap,
the individuals will only compete for resources in the
overlapping area. For the three compartment model
version we consider two independent ZOIs for a plant�s
shoot and root, representing above- and below-ground
resource uptake and competition. The ZOIs radii are
determined from the biomass of the corresponding plant
compartment.

In order to simplify spatial calculations of resource
competition, ZOIs are projected onto a grid of discrete
cells. Grid cells represent 1 cm2. The state of a grid cell is
defined by two resource availabilities, above and below
ground (in the simplified model version, only one resource
is considered). The size of the modelled arena was
128 by 128 cm. To avoid edge effects, periodic boundary
conditions were used, i.e. the grid essentially was a torus.
A model�s time step corresponds to one week; a vegetation
period consisted of 30 weeks per year, and simulations
were run for 100 years.

\section sced Process overview and scheduling
The processes resource competition, plant growth and
plant mortality are considered within each week of the
vegetation period. Seed dispersal and seedling establishment
are limited to certain weeks of the year (Table 2).
Grazing events occur randomly with a fixed probability
which is constant for all weeks. Two processes, winter
dieback of above-ground biomass and mortality of seeds
are only considered once a year, at the end of the
vegetation period (Fig. A1 in Supplementary material
Appendix 1). Plant's state variables are synchronously
updated within the subroutines for growth, mortality,
grazing and winter dieback, i.e. changes to state variables
are updated only after all model entities have been
processed (Grimm and Railsback 2005).

\section emergence Design concepts: emergence
All features observed at the community level, such as
community composition and diversity, emerged from
individual plant-plant interactions, grazing effects at the
individual scale, and resource availabilities.

\section adapt Design concepts: adaptation
In the submodel representing plant growth, aboveand
below-ground competition, plants adaptively allocate
resources to shoot and root growth in order to balance
the uptake of above- and below-ground resources (see
\ref growth "Submodel: plant growth and mortality").

\section interact Design concepts: interactions
Competitive interactions between plant individuals were
described using the ZOI approach.

\section stoch Design concepts: stochasticity
Seed dispersal and establishment, as well as mortality of
seeds and plants are modelled stochastically to include
demographic noise. Grazing events occur randomly during
the vegetation period and the affected plants are chosen
randomly, but the individual�s probability of being grazed
depends on plant traits (see Submodel: grazing).

\section obs Design concepts: observation
See section 'Design and analysis of simulation experiments'.[not in doc here]

\section init Initialization
Initially, ten seedlings of all 81 PFTs (see section Plant traits
and PFT parameterisation) with their respective seedling
mass were randomly distributed over the grid. Their
germination probability was set to 1.0 to assure equal
initial population sizes of all PFTs. A spatially and
temporally homogenous distribution of resources (both
above- and below-ground) was used in all simulation
experiments.

\section Input
The model does not include any external input of driving
environmental variables.

\section comp Submodel: competition
Following the ZOI approach, plants compete for resources
in a circular area around their central location point. To
relate plant mass to the area covered (Ashoot), we extended
the allometric relation used by (Weiner et al. 2001)

 \f{eqnarray*}{
        A_shoot &=& c_shoot \times (f_leaf \times m_shoot)^{2/3} (1)\\
   \f}

where cshoot is a constant ratio between leaf mass and ZOI
area and mshoot is vegetative shoot mass (Tables 1, 2). The
factor fleaf is introduced to describe different shoot
geometries and is defined as the ratio between photosynthetically
active (leaf) and inactive (stem) tissue (Fig. 1).

Only the former is considered for the calculation of the
ZOI size. These circular areas are projected onto a grid of
discrete cells. Grid cells thus contain the information by
which plants they are covered, so that resource competition
can be calculated cell by cell. The resources within a cell are
shared among plants according to their relative competitive
effects (bi). The resource uptake (Dres) of plant i from a cell
with resource availability (Rescell) covered by n plants is thus
calculated as

Dresi
bi
Xn
j1
bj
Rescell (2)

Calculating bi in different ways allows including different
modes of competition (Weiner et al. 2001). We assume
that the relative competitive ability of a plant is correlated
with its maximum growth rate in the absence of resource
competition. Therefore bi is proportional to maximum
resource utilization per unit area covered (rumax, see
Submodel: plant growth and mortality and Table 2). In
the case of size-symmetric competition, bi simply equals
rumax:

birumax (3a)

In the case of partially size-asymmetric competition bi is a
function of plant mass and shoot geometry:

birumaxmshootf1
leaf : (3b)

The inverse of fleaf is used, because plants with a lower
fraction of leaf tissue are considered to be higher and thus
show a higher competitive ability by overtopping other
plants (Fig. 1). In this way, plants with equal rumax receive
equal amounts of resources from one unit of area
irrespective of their mass or height in the case of sizesymmetric
competition, while larger and higher plants
receive a higher share of resources in proportion to their
shoot geometry traits in the case of partially asymmetric
competition (Schwinning and Weiner 1998, Weiner et al.
2001). The resource uptake of one plant within one week
Figure 1. Illustration of the 'zone-of-influence' (ZOI) approach
including above- and below-ground competition and different
shoot geometries. Above- and below-ground 'zones-of-influence'
are shown as light and dark grey circles, respectively. Stems and
support tissue are represented as grey cylinders. Plant individuals
compete for resources in the areas of overlap only (arrows indicate
the area of above-ground competition). The plant to the left has a
lower ratio of leaf mass to shoot mass (fleaf) and thus a smaller
above-ground ZOI. In return its competitive ability for aboveground
resources (light) is higher as it is able to shade the plant to
the right.

To include differences between intra- and interspecific
competition, individuals of the same PFT are considered
as conspecifics and those of different PFTs as heterospecifics.
The relative competitive ability bi of one plant is
then determined as a decreasing function of the number
of plants belonging to the same PFT (nPFT) and covering
the same cell:

birumax 1
ffinffiffiffiffiffiffiffiffi PFT
p (3c)

Equation 3c is used for size-symmetric competition
instead of Eq. 3a. In the case of size asymmetry, plant
mass and geometry are taken into consideration according
to Eq. 3b. This approach represents a situation where
intraspecific competition is increased relatively to interspecific
competition and therefore implicitly includes niche
differentiation of resource competition at the cell scale,
which has been known as an important factor for species
coexistence (Chesson 2000). In the model analysis,
versions with and without niche differentiation were
compared in order to test if this assumption for competition
at the cell scale translates into a different behaviour at
the community scale.

\section growth Submodel: plant growth and mortality
Plant growth only depends on the resources that the plant
acquired during the current time step (Dres). In the absence
of competition, plants show sigmoidal growth. Therefore
we adapted the growth equation used by Weiner et al.
(2001) to the description of plant geometry used here:

Dmg

Drescshootf leafrumaxm2
shoot
m4=3
max

(4)

where g is a constant conversion rate between resource
units and plant biomass and mmax is the maximum mass
of shoot or root, respectively. In addition, the maximum
amount of resources that is allocated to growth each week
is limited by a maximum resource utilization rate given by
rumax (resource units/cm2/week) multiplied by ZOI area
(cm2). If Eq. 4 yields a negative result, Dm is set to zero
and thus negative growth is prohibited.

Growth of reproductive mass is restricted to the time
between weeks 16 to 20. In this period, a constant fraction of
the resources (5% for all PFTs) is allocated to growth of
reproductive mass (Schippers et al. 2001), and reproductive
mass is limited to 5% of shoot mass in total. The same
resource conversion rate (g) is used for reproductive and
vegetative biomass.

For the two layer model version, Eq. 1.4 are applied to
shoot and root ZOIs independently, with the difference
that for root growth the factor fleaf is always one. We assume
that the minimum uptake of above- and below-ground
resources limits plant growth (Lehsten and Kleyer 2007),
and introduced adaptive shoot/root allocation in a way that
more resources are allocated to the growth of the plant
compartment that harvests the limiting resource (Weiner
2004). For resource partitioning, we adopt the model of
Johnson (1985) and the fraction of resources allocated to
shoot growth is calculated as

ashoot
DresB
DresB DresA
(5)

where DresA is above-ground and DresB is below-ground
resource uptake. To assure comparability between the one
and two layer versions, the �value� of resource units has to be
doubled in the two layer version, as resources are shared
between two plant compartments.

Plants suffer resource stress if their resource uptake (in
any layer) is below a fixed threshold fraction (thrmort) of
their optimal uptake, which is calculated as maximum
resource utilization (rumax) times ZOI area. That means
each week the condition DresBthrmortrumaxAshoot/root
is evaluated and if it is true either for shoot or root the plant
is considered as stress-exposed during this week. Consecutive
weeks of resource stress exposure (wstress) linearly
increase the probability of death

pmortpbase wstress
survmax
(6)

where survmax is the maximum number of weeks a plant
can survive under stress exposure and pbase is the
stress independent background mortality of 0.7% per
week corresponding to an annual mortality rate of 20%
(Schippers et al. 2001).

Dead plants do not grow and reproduce anymore, but
they still can shade others and are therefore still considered
for competition in the one layer model and for at least
above-ground competition in the two layer model. Each
week the mass of all dead plants is reduced by 50% and they
are removed from the grid completely as soon as their total
mass decreases below 10 mg.

\section sprod Submodel: seed production, dispersal and establishment
All plants disperse their seeds in week 21 each year. Seed
number is determined by dividing reproductive mass by the
mass of one seed (Schippers et al. 2001, Lehsten and Kleyer
2007). For each seed, dispersal distance is drawn from a lognormal
and direction from a uniform distribution (Stoyan
and Wagner 2001). To avoid edge effects on the small scale
investigated, periodic boundary conditions are used.

Germination and seedling establishment are limited to
four weeks in autumn directly after dispersal and four weeks
in spring of the next year for all PFTs. In between, a winter
mortality of 50% of seeds is assumed and all seeds which
did not germinate in these two seasons are removed.

Seedling recruitment is separated in two consecutive
processes: (1) seed germination and (2) seedling competition.
Germination is only allowed in grid cells that are not
covered by any plant or not covered in the above-ground
layer, respectively. In uncovered cells, seeds germinate with
a fixed probability (pgerm) and are converted to seedlings. In
each cell only a single plant is allowed to establish. Seedling
competition is modelled as a weighted lottery, using seed
mass as a measure of competitive ability between seedlings
(Chesson and Warner 1981, Schippers et al. 2001). The
seedling that is chosen for establishment is converted into a
plant with a shoot (and root) mass equal to seed mass. All
the other seedlings that germinated within the cell die and
are removed from the grid.

\section graz Submodel: grazing
Grazing is modelled as partial removal of an individual�s
above-ground biomass. The frequency of grazing is specified
by a constant weekly probability (pgraz) of a grazing
event. Grazing is a process that acts selectively towards
trait attributes such as shoot size and tissue properties.
Therefore, for each plant the susceptibility to grazing
(sgraz) is calculated as a function of shoot size, geometry
and PFT-specific palatability (palat).

sgrazmshootf1
leafpalat (7)

The probability for each plant to be grazed within one week
is derived by dividing individual susceptibilities by the
current maximum individual susceptibility of all plants. All
plants are checked for grazing in random order. In case a
plant is grazed, 50% of its shoot mass and its complete
reproductive mass are removed. The random choice of
plants is repeated without replacement, until 50% of the
total (above-ground) biomass on the whole grid has been
removed. When all plants have been checked for grazing
once, but less than 50% of the total above-ground biomass
has been removed, grazing probabilities for all individuals
are calculated once more based on Eq. 7 and the whole
procedure is repeated until 50% of the total above-ground
biomass has been removed or until a residual biomass is
reached which is considered ungrazable. This fraction is set
to 15 g m2 following (Schwinning and Parsons 1999).
This allows a plant individual to be grazed never or several
times during one week with a grazing event.

In addition to stochastic grazing, each year at the end of
the vegetation period 50% of the above-ground mass of all
plant individuals is removed to mimic vegetation dieback in
winter.

\section traits Plant traits and PFT parameterisation
The PFTs that are used in the model differ in their
attributes for nine plant functional traits (Table 2). These
are grouped into four trait syndromes based on well
documented tradeoffs and trait correlations. The traits we
chose comprise a subset of the 'common core list of plant
traits', proposed by Weiher et al. (1999).

The growth form of a plant is characterized by the ratio
between leaf mass and total shoot mass (fleaf). Plants with a
low fleaf use more biomass to build up support tissue (e.g.
stems) instead of leave mass. Therefore this trait implicitly
includes a tradeoff between plant height and leaf area or
between competitive ability for light versus relative growth
rate without competition, respectively (Fig. 1; Eq. 1, 3b).

Maximum plant mass is the second trait that describes
plant geometry and is positively related to actual plant
mass, according to the sigmoidal growth equation (Eq. 4).
In the two-layer version equal maximal masses for shoot
and root are assumed. Individual susceptibility to grazing
is modelled as a function of fleaf and mshoot (Eq. 7) as
higher and larger plants tend to be more affected by
grazing (Diaz et al. 2001).

Maximum plant mass and individual seed mass are
assumed to be positively correlated and combined within a
trait syndrome (Eriksson and Jakobsson 1998). The welldocumented
seed size versus seed number tradeoff emerges
in our model, because PFTs with higher seed mass produce
less seeds from the same amount of reproductive mass
(Leishman 2001, Schippers et al. 2001, Westoby et al.
2002). The disadvantage of a low seed number is balanced
by the higher recruitment success of large seedlings, due to
the weighted lottery of seedling competition and the higher
initial mass of plants that germinated from large seeds
(Jakobsson and Eriksson 2000). A negative correlation
between seed mass and dispersal distance was assumed
here, which is supported at least for wind-dispersed seeds
(Jongejans and Schippers 1999). For simplicity, equal
values are used for mean and standard deviation of the
dispersal kernel assuming a higher variance for the dispersal
distances of smaller seeds.

The response of plants to different resource conditions is
distinguished into categories reflecting the tradeoff between
competitive ability and stress tolerance (Grime 2001).

Maximum growth rate in resource-rich environments is
given as the maximum resource utilization per shoot/root
area (rumax), while stress resistance is specified by the
maximal number of weeks a plant can survive under
resource stress exposure (survmax). Accordingly, PFTs with
high rumax feature a low stress resistance and vice versa.

We distinguished two strategies of response to grazing:
(1) grazing tolerance by fast re-growth of removed biomass
and (2) grazing avoidance by low palatability through
defence structures or secondary compounds (Bullock et al.
2001). In our model, the relationship between leaf mass and
leaf area is given by the parameter cshoot (Eq. 1). This
parameter is functionally analogous to the specific leaf area
(SLA), which was proposed by Westoby et al. (2002) as a
key trait to characterize plant strategies. High SLA is related
to high efficiency of light interception and fast growth,
while leaves with low SLA show higher longevity, structural
strength or high allocation to defensive compounds
(Westoby et al. 2002). Accordingly, a high cshoot value for
grazing tolerance versus a low value for grazing avoidance is
used here. The positive effect of defence compounds is
expressed as a low palatability (palat) and thus, a low
susceptibility towards grazing (Eq. 7).

For each of the four functional traits three attributes
were distinguished. The parameter values used should
represent the range of contrasting plant strategies with
respect to resource response, competitive ability and
grazing response that could potentially occur in grassland
ecosystems. Combining all three attributes of all four
functional traits allowed us to define 34 by 81 PFTs in total
(Table 3).

source: \ref bib "May et al. (2009)"
*/