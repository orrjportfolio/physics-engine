#include "map.hpp"

namespace Map {
	static Tex
		sandTex,
		rockTex,
		roadTex;
	
	static auto
		waterMaterial = Material{.kind = Material::KIND_LIT_UNTEXED, .colour = glm::vec3(0.2f, 0.2f, 1.0f)},
		sandMaterial = Material{.kind = Material::KIND_LIT, .tex = &sandTex, .colour = glm::vec3(1.0f)},
		rockMaterial = Material{.kind = Material::KIND_LIT, .tex = &rockTex, .colour = glm::vec3(1.0f)},
		roadMaterial = Material{.kind = Material::KIND_LIT, .tex = &roadTex, .colour = glm::vec3(1.0f)};
	
	static auto
		waterPhysMaterial = PhysicsMaterial{.sFrict = 0.1f, .dFrict = 0.08f, .bounciness = 0.2f},
		sandPhysMaterial = PhysicsMaterial{.sFrict = 1.0f, .dFrict = 0.8f, .bounciness = 0.1f},
		rockPhysMaterial = PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f},
		roadPhysMaterial = PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f};
	
	Object objects[] = {
		Object{
			.pos = glm::vec3(72.14353942871094, -12.675759315490723, -84.04740905761719),
			.halfSize = glm::vec3(81.20398712158203, 13.91763973236084, 48.466678619384766),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &sandMaterial, .physMaterial = sandPhysMaterial
		},
		Object{
			.pos = glm::vec3(194.66400146484375, -12.675800323486328, -6.6838698387146),
			.halfSize = glm::vec3(42.89680099487305, 13.91759967803955, 125.83000183105469),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &sandMaterial, .physMaterial = sandPhysMaterial
		},
		Object{
			.pos = glm::vec3(171.1544952392578, -12.675759315490723, 75.98370361328125),
			.halfSize = glm::vec3(66.40506744384766, 13.91763973236084, 48.15532684326172),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &sandMaterial, .physMaterial = sandPhysMaterial
		},
		Object{
			.pos = glm::vec3(100.86785888671875, -21.18467903137207, -13.391021728515625),
			.halfSize = glm::vec3(35.57855224609375, 13.917638778686523, 27.677928924560547),
			.rot = glm::vec3(-0.03460981696844101, 1.0396381616592407, -0.006789491977542639),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-78.64625549316406, 4.310702323913574, -18.05618667602539),
			.halfSize = glm::vec3(1.5, 1.0, 2.0),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(25.762062072753906, -6.7780303955078125, -48.31315612792969),
			.halfSize = glm::vec3(35.72803497314453, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(98.1156005859375, -6.7780303955078125, -48.31315612792969),
			.halfSize = glm::vec3(19.632369995117188, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(30.02977752685547, 24.210693359375, 7.836994171142578),
			.halfSize = glm::vec3(81.20398712158203, 5.935290336608887, 71.12191772460938),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-45.458282470703125, -6.7780303955078125, -41.20360565185547),
			.halfSize = glm::vec3(42.51703643798828, 29.027881622314453, 30.239574432373047),
			.rot = glm::vec3(0.0, 0.6755036115646362, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-54.53013229370117, -7.138576507568359, 21.855911254882812),
			.halfSize = glm::vec3(34.05762481689453, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 1.7880891561508179, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-28.005674362182617, -7.138576507568359, 61.44306564331055),
			.halfSize = glm::vec3(34.05762481689453, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 2.532139778137207, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(9.5341796875, -7.138574600219727, 72.884765625),
			.halfSize = glm::vec3(34.05762481689453, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 3.241480827331543, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(54.77394104003906, -7.138572692871094, 61.076229095458984),
			.halfSize = glm::vec3(34.0576286315918, 29.027881622314453, 14.971768379211426),
			.rot = glm::vec3(0.0, 3.785595417022705, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(71.44143676757812, -7.138572692871094, 43.285831451416016),
			.halfSize = glm::vec3(34.05762481689453, 29.027881622314453, 14.971766471862793),
			.rot = glm::vec3(0.0, 4.163740158081055, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(136.86769104003906, -6.7780303955078125, -28.160091400146484),
			.halfSize = glm::vec3(37.82585525512695, 29.027881622314453, 18.6165771484375),
			.rot = glm::vec3(0.0, -0.7869148850440979, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(164.44711303710938, -6.7780303955078125, 27.768054962158203),
			.halfSize = glm::vec3(13.149979591369629, 29.027881622314453, 37.05488586425781),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(96.29037475585938, -6.778029441833496, 41.06000900268555),
			.halfSize = glm::vec3(13.472984313964844, 29.027881622314453, 31.111576080322266),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(124.79679870605469, -6.7780303955078125, 93.29739379882812),
			.halfSize = glm::vec3(37.82585525512695, 29.027881622314453, 14.97176742553711),
			.rot = glm::vec3(0.0, -0.7869148850440979, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(155.2063751220703, -6.7780303955078125, 107.52813720703125),
			.halfSize = glm::vec3(22.39072036743164, 29.027881622314453, 16.610897064208984),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(96.39311218261719, 24.210693359375, 57.99522018432617),
			.halfSize = glm::vec3(81.20398712158203, 5.935290336608887, 71.12191772460938),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(105.26457214355469, 24.210693359375, 35.07101821899414),
			.halfSize = glm::vec3(39.03746795654297, 5.935290336608887, 71.12191772460938),
			.rot = glm::vec3(0.0, -0.34375977516174316, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(91.74470520019531, 45.62754440307617, 24.349708557128906),
			.halfSize = glm::vec3(51.82564926147461, 13.089639663696289, 45.391117095947266),
			.rot = glm::vec3(0.0, -0.7412782311439514, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(98.36415100097656, 45.62754440307617, 59.00617599487305),
			.halfSize = glm::vec3(51.825653076171875, 13.089639663696289, 45.39112091064453),
			.rot = glm::vec3(0.0, -1.9213383197784424, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(40.111602783203125, 45.62754440307617, 5.79326057434082),
			.halfSize = glm::vec3(51.825653076171875, 13.089639663696289, 45.39112091064453),
			.rot = glm::vec3(0.0, -1.574926495552063, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(205.04800415039062, 0.39953601360321045, 12.962200164794922),
			.halfSize = glm::vec3(5.8112006187438965, 0.9111649990081787, 49.192649841308594),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(151.89500427246094, 0.39953699707984924, -72.53309631347656),
			.halfSize = glm::vec3(6.278600215911865, 0.9111649990081787, 46.904449462890625),
			.rot = glm::vec3(0.0, 1.08108389377594, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(97.8309326171875, 0.39953717589378357, -94.27507019042969),
			.halfSize = glm::vec3(5.8611040115356445, 0.9111650586128235, 15.625581741333008),
			.rot = glm::vec3(0.0, 1.570804238319397, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(70.33515930175781, -10.652300834655762, -57.194942474365234),
			.halfSize = glm::vec3(7.281458854675293, 11.963003158569336, 25.500049591064453),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(77.54048156738281, 0.39953717589378357, -86.02214050292969),
			.halfSize = glm::vec3(7.294270992279053, 0.9111650586128235, 12.951414108276367),
			.rot = glm::vec3(0.0, 2.309443950653076, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(197.7949981689453, 0.39953699707984924, -41.86880111694336),
			.halfSize = glm::vec3(7.154749393463135, 0.9111649990081787, 12.298250198364258),
			.rot = glm::vec3(0.0, 0.6364133954048157, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(56.179931640625, -11.925107955932617, 27.279964447021484),
			.halfSize = glm::vec3(6.660404682159424, 13.235810279846191, 21.595291137695312),
			.rot = glm::vec3(0.0, 2.6031243801116943, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(34.018829345703125, -11.854669570922852, 47.90024185180664),
			.halfSize = glm::vec3(6.325621604919434, 13.165371894836426, 15.625581741333008),
			.rot = glm::vec3(0.0, 1.8761922121047974, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(7.251716613769531, -13.428569793701172, 53.055538177490234),
			.halfSize = glm::vec3(5.461663246154785, 12.535667419433594, 15.625581741333008),
			.rot = glm::vec3(-0.1276770681142807, 1.577225923538208, -0.0691053494811058),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-18.423049926757812, -17.316707611083984, 44.26152801513672),
			.halfSize = glm::vec3(5.461663246154785, 12.535667419433594, 15.625581741333008),
			.rot = glm::vec3(-0.10496411472558975, 0.9489116668701172, -0.047788091003894806),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-32.08686828613281, -20.12508201599121, 22.558422088623047),
			.halfSize = glm::vec3(5.461663246154785, 12.535667419433594, 15.62558364868164),
			.rot = glm::vec3(-0.08858339488506317, 0.20062915980815887, -0.003905376186594367),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(-22.31109619140625, -20.251251220703125, -1.7020626068115234),
			.halfSize = glm::vec3(4.959654331207275, 11.649066925048828, 19.574758529663086),
			.rot = glm::vec3(0.0, -0.8715988397598267, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(30.88159942626953, -9.101919174194336, -14.301918029785156),
			.halfSize = glm::vec3(3.794771671295166, 1.1561261415481567, 41.376739501953125),
			.rot = glm::vec3(-0.01753373257815838, 1.570804238319397, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(132.4722137451172, -11.413554191589355, 42.54972457885742),
			.halfSize = glm::vec3(7.402022361755371, 12.72425651550293, 15.312112808227539),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(137.7364959716797, 0.39953717589378357, 66.30708312988281),
			.halfSize = glm::vec3(6.77636194229126, 0.9111650586128235, 12.298233032226562),
			.rot = glm::vec3(0.0, 0.4970385730266571, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(198.8840789794922, 0.39953717589378357, 67.85261535644531),
			.halfSize = glm::vec3(4.959654331207275, 0.9111650586128235, 12.29823112487793),
			.rot = glm::vec3(0.0, -0.7433378100395203, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(181.5758514404297, 0.39953717589378357, 79.49674987792969),
			.halfSize = glm::vec3(4.959654331207275, 0.9111650586128235, 11.648009300231934),
			.rot = glm::vec3(0.0, -1.2288293838500977, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(154.0904998779297, 0.39953717589378357, 79.10630798339844),
			.halfSize = glm::vec3(5.944164752960205, 0.9111650586128235, 14.001175880432129),
			.rot = glm::vec3(0.0, 1.2556352615356445, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(169.06520080566406, 0.39953717589378357, 83.20133972167969),
			.halfSize = glm::vec3(4.959654331207275, 0.9111650586128235, 3.200345993041992),
			.rot = glm::vec3(0.0, 1.5707963705062866, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(33.30387878417969, -12.476027488708496, 30.432170867919922),
			.halfSize = glm::vec3(3.8357815742492676, 11.963003158569336, 3.268951416015625),
			.rot = glm::vec3(-0.11717581748962402, 0.22024978697299957, -0.09704422950744629),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(32.82634735107422, -16.008865356445312, 13.238468170166016),
			.halfSize = glm::vec3(3.8357815742492676, 11.963003158569336, 3.268951892852783),
			.rot = glm::vec3(-0.08254716545343399, -0.36481279134750366, -0.031373221427202225),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(41.170318603515625, -19.27907371520996, -1.6208057403564453),
			.halfSize = glm::vec3(3.835782051086426, 11.963004112243652, 3.268951416015625),
			.rot = glm::vec3(-0.029758496209979057, 0.6130604147911072, -0.1209397241473198),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &rockMaterial, .physMaterial = rockPhysMaterial
		},
		Object{
			.pos = glm::vec3(67.71720886230469, 0.3950122594833374, -10.731998443603516),
			.halfSize = glm::vec3(7.281455993652344, 0.9156899452209473, 22.06253433227539),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(59.859710693359375, 2.2498068809509277, -11.659418106079102),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(75.66856384277344, 2.2498068809509277, -9.820384979248047),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(59.86183166503906, 4.812499046325684, -11.677631378173828),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(75.67068481445312, 4.812499046325684, -9.838600158691406),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(59.863922119140625, 7.333250999450684, -11.695547103881836),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(75.67277526855469, 7.333250999450684, -9.856515884399414),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(59.866058349609375, 9.925382614135742, -11.713970184326172),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(75.67491149902344, 9.925382614135742, -9.87493896484375),
			.halfSize = glm::vec3(0.21578383445739746, 0.9156897068023682, 22.06252670288086),
			.rot = glm::vec3(-0.007155155763030052, -0.11580867320299149, 0.0),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(240.1205291748047, -1.7246578931808472, -245.5150146484375),
			.halfSize = glm::vec3(369.6255798339844, 1.9189164638519287, 188.93142700195312),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_NONE,
			.material = &waterMaterial
		},
		Object{
			.pos = glm::vec3(425.0775146484375, -1.7246578931808472, 233.58731079101562),
			.halfSize = glm::vec3(311.1964111328125, 1.9189164638519287, 188.93142700195312),
			.rot = glm::vec3(0.0, 1.5707963705062866, 0.0),
			.colliderKind = COLLIDER_KIND_NONE,
			.material = &waterMaterial
		},
		Object{
			.pos = glm::vec3(69.37049865722656, -1.7246578931808472, 308.4422607421875),
			.halfSize = glm::vec3(208.97076416015625, 1.9189164638519287, 188.93142700195312),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_NONE,
			.material = &waterMaterial
		},
		Object{
			.pos = glm::vec3(-270.3050537109375, -1.7246578931808472, 28.250118255615234),
			.halfSize = glm::vec3(225.03623962402344, 1.9189164638519287, 462.40789794921875),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_NONE,
			.material = &waterMaterial
		},
		Object{
			.pos = glm::vec3(-23.75653076171875, -16.5501651763916, 46.724849700927734),
			.halfSize = glm::vec3(130.78656005859375, 1.9189164638519287, 188.93142700195312),
			.rot = glm::vec3(0.0, 0.0, -0.0),
			.colliderKind = COLLIDER_KIND_NONE,
			.material = &waterMaterial
		},
		
		Object{
			.pos = glm::vec3(59.5374f, 13.0859f, -11.5825),
			.halfSize = glm::vec3(0.831568f, 25.4141f, 48.2928f) / 2.0f,
			.rot = glm::vec3(glm::radians(-0.40996f), glm::radians(-6.63535f), 0.0f),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.hidden = true,
			//.material = &roadMaterial,
			.physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(76.2762f, 13.0859f, -9.58261),
			.halfSize = glm::vec3(0.831568f, 25.4141f, 48.2928f) / 2.0f,
			.rot = glm::vec3(glm::radians(-0.40996f), glm::radians(-6.63535f), 0.0f),
			.colliderKind = COLLIDER_KIND_KINEMATIC,
			.hidden = true,
			//.material = &roadMaterial,
			.physMaterial = roadPhysMaterial
		}
	};
	
	void init() {
		sandTex = Tex::load("assets/textures/sand.png", Tex::FLAG_FILTER | Tex::FLAG_MIPMAP);
		rockTex = Tex::load("assets/textures/rock.png", Tex::FLAG_FILTER | Tex::FLAG_MIPMAP);
		roadTex = Tex::load("assets/textures/road.png", Tex::FLAG_FILTER | Tex::FLAG_MIPMAP);
	}
}
