class DukeAtomicHealth : DukeItemBase
{
	default
	{
		pic "ATOMICHEALTH";
	}
	
	override void Initialize()
	{
		commonItemSetup();
		self.cstat |= CSTAT_SPRITE_YCENTER;
	}
	
	override bool animate(tspritetype t)
	{
		t.pos.Z -= 4;
		t.shade = -127;
		return false;
	}
}

class RedneckGoogooCluster : DukeItemBase
{
	default
	{
		pic "ATOMICHEALTH";
	}
	
	override void Initialize()
	{
		commonItemSetup((0.125, 0.125));
		self.cstat |= CSTAT_SPRITE_YCENTER;
	}
	
	override bool animate(tspritetype t)
	{
		t.pos.Z -= 4;
		t.shade = -127;
		return false;
	}
	
}
