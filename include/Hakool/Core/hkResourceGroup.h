#pragma once

#include <Hakool\Utils\hkUtilitiesUtilities.h>
#include <Hakool\Utils\hkLogger.h>

#include <Hakool\Core\hkCorePrerequisites.h>
#include <Hakool\Core\hkResource.h>

namespace hk
{
  /**
  * TODO
  */
  template<class C>
  class ResourceGroup
  {
  public:

    /**
    * Constructor.
    */
    ResourceGroup();

    /**
    * Destructor.
    */
    ~ResourceGroup();

    /**
    * Add an asset and pass its memory management to this resource group. If 
    * there exists an asset with the same identifier, then the new asset will 
    * be rejected.
    * 
    * @param _key The identifier of the asset.
    * @param _pAsset The pointer of the asset.
    * 
    * @return
    *
    * * kSuccess: The asset was added.
    * * kObjectAlreadyExists: An asset with the same identifier exists.
    */
    eRESULT
    add(const String& _key, C* _pAsset);

    /**
    * Get a resource from this group. May return a null pointer if the key was
    * not found.
    * 
    * @param _key The identifier of the asset.
    * 
    * @return The pointer to the asset. Returns a null pointer if the asset was
    * not found.
    */
    C*
    get(const String& _key);

    /**
    * Check if the given identifier exists in the group.
    * 
    * @param _key The identifier of the asset.
    * 
    * @return True if the asset exists, otherwise returns false.
    */
    bool
    has(const String& _key);

    /**
    * Removes and destroy an asset.
    * 
    * @param _key The identifier of the asset.
    * 
    * @return Operation result.
    */
    eRESULT
    removeAndDestroy(const String& _key);

    /**
    * Remove and destroy all the assets.
    */
    void
    clear();

  private:


    /**
    * Table of resources, each one identified by a unique string key.
    */
    Map<String, Resource*>
    _m_hResources;    
  };

  template<class C>
  inline ResourceGroup<C>::ResourceGroup() : 
    _m_hResources()
  {
    return;
  }

  template<class C>
  inline ResourceGroup<C>::~ResourceGroup()
  {
    clear();
    return;
  }

  template<class C>
  inline eRESULT ResourceGroup<C>::add(const String& _key, C* _pAsset)
  {
    static_assert(IsBaseOf<Resource, C>::value, "Class must derive from Resource");
    
    Resource* pResource = reinterpret_cast<Resource*>(_pAsset);

    if (pResource == nullptr)
    {
      Logger::Error("| ResourceGroup | Invalid asset pointer.");
      return eRESULT::kFail;
    }

    if (has(_key))
    {
      Logger::Error
      (
        "| ResourceGroup | Asset with key : " 
        + _key 
        + " already exists. Asset will be destroyed"
      );

      pResource->destroy();
      delete pResource;

      return eRESULT::kObjectAlreadyExists;
    }

    _m_hResources.insert
    (
      Map<String, Resource*>::value_type(_key, pResource)
    );

    return eRESULT::kSuccess;
  }

  template<class C>
  inline C* 
  ResourceGroup<C>::get(const String& _key)
  {
    if (!has(_key))
    {
      Logger::Error
      (
        "| ResourceGroup | Asset with key : " 
        + _key 
        + " was not found."
      );

      return nullptr;
    }

    return reinterpret_cast<C*>(_m_hResources.find(_key)->second);
  }

  template<class C>
  inline bool 
  ResourceGroup<C>::has(const String& _key)
  {
    return _m_hResources.find(_key) != _m_hResources.end();
  }

  template<class C>
  inline eRESULT 
    ResourceGroup<C>::removeAndDestroy(const String& _key)
  {
    Resource* pResource = get(_key);

    _m_hResources.erase(_key);

    if (pResource != nullptr)
    {
      return eRESULT::kObjectNotFound;
    }

    pResource->destroy();
    delete pResource;

    return eRESULT::kSuccess;
  }

  template<class C>
  inline void 
  ResourceGroup<C>::clear()
  {
    for (auto iterator : _m_hResources)
    {
      Resource* pResource = iterator.second;
      pResource->destroy();
      delete pResource;
    }
    _m_hResources.clear();

    return;
  }
}