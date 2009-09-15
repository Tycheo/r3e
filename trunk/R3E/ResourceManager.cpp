#include "ResourceManager.hpp"
#include "ZMS.hpp"
#include "ZMO.hpp"
#include "ZMD.hpp"
#include "Texture.hpp"

ResourceManager<ROSE::ZMS> ResourceManager<ROSE::ZMS>::mInstance;
ResourceManager<ROSE::ZMO> ResourceManager<ROSE::ZMO>::mInstance;
ResourceManager<ROSE::ZMD> ResourceManager<ROSE::ZMD>::mInstance;
ResourceManager<Texture> ResourceManager<Texture>::mInstance;
