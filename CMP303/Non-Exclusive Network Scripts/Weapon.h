#ifndef WEAPON_H
#define WEAPON_H


#include "Sprites.h"
#include "Bullet.h"

#include "Framework\AudioManager.h"

/// <summary>
/// The weapon class is the parent of all weapons, it contains updates, spawn, render functions. It also contains the bullet class.
/// </summary>
class Weapon :
    public Sprites
{
private:
    /// <summary>
/// If the bullets are outside of the viewport, make them dead.
/// </summary>
    void deathCheck(int i);
    sf::Font font;//To replace elsewhere
public:
    Weapon();
    ~Weapon();

    /// <summary>
    /// Updates the bullets and deathchecks them.
    /// </summary>
    void update(float dt) override;
	/// <summary>
	/// Draws all alive bullets on screen. This will have to be called from player->equippedWeapon.render()
	/// </summary>
	void render();
    /// <summary>
    /// Controls shooting, and uses player position and direction for instantiating the bullets
    /// </summary>
    void virtual handleInput(float, sf::Vector2f, sf::Vector2f);

    void Reload();

    std::vector<Bullet*> bullets;  
    
    AudioManager* aud;

    sf::Text magazine;//Text for displaying the bullets in magazine and magazine size to the player
    sf::Text reload;//Text for displaying the bullets in magazine and magazine size to the player

    void RepositionUI();

#pragma region Setters/Getters
    void SetWeaponImage(sf::Texture* image); 
    void SetBulletsLeft(int i) { bulletsLeft = i; };
    

    sf::Texture* GetWeaponImage() { return weaponImage; };
    
    bool GetIsShooting() { return isShooting; };
    int GetBulletsLeft() { return bulletsLeft; };    int GetMaxBullets() { return maxBullets; };
    int GetBulletsInMag() { return bulletsInMag; };     int GetMaxBulletsInMag() { return maxBulletsInMag; };
    int GetCost() { return cost; };
    float GetDamage() { return damage; };

    std::string GetName() { return name; };
    void spawn(sf::Vector2f bulletSpawnPos, sf::Vector2f playerLookDirec);
#pragma endregion
protected:

    /// <summary>
/// Spawns a bullet with a direction and position.
/// </summary>
    bool isShooting;

    int bulletsLeft;//Amount of bullets left available for reload 
    int maxBullets;//Maximum amount of bullets that can be in 'bulletsLeft'
    float damage;
    int bulletsInMag;//Amount of bullets in mag currently fireable.
    int maxBulletsInMag;;//Max amount of bulletes that can be in 'bulletsInMag'

    std::string name;
    int cost;//Cost to purchase, to get cost for ammo do (cost * 0.5 + 30) 

    sf::Texture* weaponImage; //Image used for inventory screen
    sf::Texture bulletTexture;



    void SetupOneBullet(Bullet*, sf::Texture*);
};

#endif // !WEAPON_H