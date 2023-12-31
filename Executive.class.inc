<?php

require_once('Area.class.inc');
require_once('Game.class.inc');
require_once('Player.class.inc');

function incrementArrayKey($pKey, &$pArray, $pIncrementBy = 1)
{
    if (!isset($pArray[$pKey])) $pArray[$pKey] = 0;
    
    $pArray[$pKey] += $pIncrementBy;
}

class Executive
{
	const DOOR = 'door';
	const NORTH = 'north';
	const SOUTH = 'south';
	const EAST = 'east';
	const WEST = 'west';
	const DOWN = 'down';
    const UP = 'up';
    const LEFT = 'left';
    const RIGHT = 'right';

	private $game;
	private $player;
	private $area;
	
	private $tags; 
	
	public function __construct()
	{
		$this->game = Game::get();
		$this->player = Player::get();
		$this->area = $this->game->getArea($this->player->location);
		
		$this->clearTags();
	}
	
	public static function get()
	{
		if (!isset($_SESSION['executive']))
		{
			$_SESSION['executive'] = new Executive();
		}
		
		return $_SESSION['executive'];
	}
	
	private function clearTags()
	{
		$this->tags = array();
	}
	
	

	
	public function look()
	{	
		return $this->areaDescription();
	}
	
	public function go($pDirection)
	{
		$area = $this->game->getArea($this->player->location);
		

		$votesByOutletIndex = array();
		
		for ($i=0; $i<count($area->outlets); $i++)
		{	
			$outlet = $area->outlets[$i];
			foreach ($outlet->tags as $tag)
			{	
                debug('Tag: '.$tag.', Direction: '.$pDirection);
				if ($tag == '@'.$pDirection)
				{
					incrementArrayKey($i, $votesByOutletIndex);
				}
			}
		}
        debug('Area outlets:');
		debug($area->outlets);
        
		krsort($votesByOutletIndex);
		debug('Votes by outlet index:');
		debug($votesByOutletIndex);
		
        foreach ($votesByOutletIndex as $index => $votes)
        {



            $outlet = $area->outlets[$index];
            $this->player->location = $outlet->destArea;
            
            return $this->areaDescription();
        }				
		

		return 'Not sure how to go that way.';
	}
    
    public function pickUp($pObjWords)
    {
        $cmdNoun = array_pop($pObjWords);
        $cmdAdjs = $pObjWords;
        
        $area = $this->game->getArea($this->player->location);
        $objects = $area->objects;
        
        debug('Objects:');
        debug($objects);
        
        $votesByObjectName = array();
        
        foreach ($objects as $objName)
        {
            $object = $this->game->getObject($objName);
            debug('Object "'.$objName.'"');
            debug($object);
            
            foreach ($object->nouns as $objNoun)
            {
                if ($cmdNoun == $objNoun)
                {

					incrementArrayKey($objName, $votesByObjectName, 10);
                }
            }
            
            foreach ($object->adjs as $objAdj)
            {
                foreach ($cmdAdjs as $cmdAdj)
                {
                    if ($cmdAdj == $objAdj)
                    {

                        incrementArrayKey($objName, $votesByObjectName, 1);
                    }                
                }
            }
        }
        
        krsort($votesByObjectName);
        
        foreach ($votesByObjectName as $objName => $votes)
        {



            $object = $this->game->getObject($objName);
            
            if ($object->canCarry)
            {

                $this->player->addToInventory($objName);
                // Remove from area 
                $area->objects = array_values(
                    array_diff($objects, array($objName))
                );
                
                
                return $object->carryStr?
                    $object->carryStr:
                    ('You pick up the '.$object->title.'.');
            }
            else
            {
                return $object->carryStr?
                    $object->carryStr:
                    ('You cannot carry the '.$object->title.'.');
            }
        }				
        
		return 'I don\'t see any '.$cmdNoun.' here.';
    }
    
    public function drop()
    {
        $invObjNames = $this->player->getInventory();
        
        if (!count($invObjNames))
        {
            return 'You\'re not carrying anything.';
        }
        
        return 'I haven\'t implemented that yet!';
    }
    
    public function showInventory()
    {
        $invObjNames = $this->player->getInventory();
        
        if (count($invObjNames))
        {
            $Output = "You are carrying: \n";
            foreach ($invObjNames as $objName)
            {
                $object = $this->game->getObject($objName);
                $Output .= "- ".ucfirst($object->title)." \n";
            }
            return trim($Output);
        }
        else
        {
            return 'You are empty-handed.';
        }
    }
	
	private function areaDescription()
	{
        debug('Location:');
        debug($this->player->location);
		$this->area = $this->game->getArea($this->player->location);
		
		$output = $this->area->description;
        debug('Area:');
        debug($this->area);
		
		$objectOutput = '';
		foreach ($this->area->objects as $objectName)
		{
			$object = $this->game->getObject($objectName);
			if ($object->tagline)
			{
				$objectOutput .= $object->tagline."\n";
			}
		}
		
		if ($objectOutput)
		{
			$output .= "\n\n".trim($objectOutput);
		}
		
		return $output;
	}
}






























