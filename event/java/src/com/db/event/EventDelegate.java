/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.event;

import com.db.util.MethodInvoker;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;

/**
 * This class is used to delegate events to listeners.
 * 
 * @author Dave Longley
 */
public class EventDelegate
{
   /**
    * A list of all of the listeners of this delegate.
    */
   protected Vector mListeners;
   
   /**
    * A map of listener to the method to call to handle an event.
    */
   protected HashMap mListenerToMethod;

   /**
    * Constructs a new event delegate.
    */
   public EventDelegate()
   {
      // create listener list
      mListeners = new Vector();
      
      // create listener to method map
      mListenerToMethod = new HashMap();
   }
   
   /**
    * Adds a listener.
    *
    * @param listener the listener to add.
    * @param method the name of the listener method to call to handle an event.
    */
   public synchronized void addListener(Object listener, String method)
   {
      if(!hasListener(listener))
      {
         mListeners.add(listener);
         mListenerToMethod.put(listener, method);
      }
   }
   
   /**
    * Removes a listener.
    * 
    * @param listener the listener to remove.
    */
   public synchronized void removeListener(Object listener)
   {
      if(hasListener(listener))
      {
         mListeners.remove(listener);
         mListenerToMethod.remove(listener);
      }
   }
   
   /**
    * Returns true if the passed listener is already listening for
    * events fired by this delegate, false if not.
    *
    * @param listener the listener to check for.
    * 
    * @return true if the listener is already listening to this delegate,
    *         false if not. 
    */
   public synchronized boolean hasListener(Object listener)
   {
      boolean rval = false;
      
      if(mListeners.contains(listener))
      {
         rval = true;
      }
      
      return rval;
   }
   
   /**
    * Fires an event to all listeners.
    * 
    * @param event the event to fire.
    */
   public synchronized void fireEvent(Object event)
   {
      Iterator i = mListeners.iterator();
      while(i.hasNext())
      {
         Object listener = i.next();
         
         // store event as a parameter to the listener method 
         Object[] params = new Object[]{event};
         
         // get the listener method
         String method = (String)mListenerToMethod.get(listener);
         
         // fire message
         MethodInvoker mi =
            new MethodInvoker(listener, method, params);
         mi.execute();
      }
   }
}
