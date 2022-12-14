class DukeCommander : DukeActor
{
	default
	{
		pic "COMMANDER";
	}
	
	override void PlayFTASound()
	{
		self.PlayActorSound("COMM_RECOG");
	}
}

class DukeCommanderStayput: DukeCommander
{
	default
	{
		pic "COMMANDERSTAYPUT";
	}
	
	override void initialize()
	{
		super.initialize();
		self.actorstayput = self.sector;	// make this a flag once everything has been exported.
	}
}