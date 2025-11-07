# baseq3a

Unofficial Quake III Arena gamecode patch

# What is done:

 * new toolchain used (optimized q3lcc and q3asm)
 * upstream security fixes
 * floatfix
 * fixed vote system
 * fixed spawn system
 * fixed in-game crosshair proportions
 * fixed UI mouse sensitivity for high-resolution
 * fixed not being able to gib after match end (right before showing the scores)
 * fixed shotgun not gibbing unless aiming at the feet
 * improved gibs physics, new CVARs
   * `cg_oldGibs`
   * `cg_gibsInheritPlayerVelocity`
   * `cg_gibsExtraRandomVelocity`
   * `cg_gibsExtraVerticalVelocity`
 * fixed server browser + faster scanning
 * fixed grappling hook muzzle position visuals
 * new demo UI (subfolders,filtering,sorting)
 * updated serverinfo UI
 * map rotation system
 * unlagged weapons
 * improved prediction
 * damage-based hitsounds
 * colored skins
 * high-quality proportional font renderer
 * single-line cvar declaration, improved cvar code readability and development efficiency
 * single-line event (EV_*) declaration
 * single-line mean of death (MOD_*) declaration

# TODO:

 * bugfixes

# Documentation

See /docs/

# Compilation and installation

Look in /build/
