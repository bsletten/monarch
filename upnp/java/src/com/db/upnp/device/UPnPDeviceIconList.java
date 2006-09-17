/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.upnp.device;

import java.util.Iterator;
import java.util.Vector;

import com.db.logging.Logger;
import com.db.logging.LoggerManager;
import com.db.xml.AbstractXmlSerializer;
import com.db.xml.XmlElement;

/**
 * A UPnPDeviceIconList represents a list of UPnPDeviceIcons.
 * 
 * The following is taken from:
 * 
 * http://www.upnp.org/download/UPnPDA10_20000613.htm
 * 
 * -----------------------------------------------------------------------
 * <pre>
 * 
 * <iconList>
 *   <icon>
 *     <mimetype>image/format</mimetype>
 *     <width>horizontal pixels</width>
 *     <height>vertical pixels</height>
 *     <depth>color depth</depth>
 *     <url>URL to icon</url>
 *   </icon>
 *   XML to declare other icons, if any, go here
 * </iconList>
 * 
 * </pre>
 * 
 * iconList
 * Required if and only if device has one or more icons. Specified by UPnP
 * vendor. Contains the following sub elements:
 * 
 * icon
 * Recommended. Icon to depict device in a control point UI. May be localized
 * (cf. ACCEPT-/CONTENT-LANGUAGE headers). Recommend one icon in each of the
 * following sizes (width x height x depth): 16x16x1, 16x16x8, 32x32x1,
 * 32x32x8, 48x48x1, 48x48x8.
 * -----------------------------------------------------------------------
 * 
 * @author Dave Longley
 */
public class UPnPDeviceIconList extends AbstractXmlSerializer
{
   /**
    * The UPnPDeviceIcons for this list.
    */
   protected Vector mIcons;
   
   /**
    * Creates a new UPnPDeviceIconList.
    */
   public UPnPDeviceIconList()
   {
      // create the icon list
      mIcons = new Vector();
   }
   
   /**
    * Returns the root tag name for this serializer.
    * 
    * @return the root tag name for this serializer.
    */
   public String getRootTag()   
   {
      return "iconList";
   }
   
   /**
    * Creates an XmlElement from this object.
    *
    * @param parent the parent XmlElement for the XmlElement being created
    *               (can be null). 
    * 
    * @return the XmlElement that represents this object.
    */
   public XmlElement convertToXmlElement(XmlElement parent)   
   {
      // create the root element
      XmlElement listElement = new XmlElement(getRootTag());
      listElement.setParent(parent);
      
      // convert each icon to an xml element child
      for(Iterator i = getIcons().iterator(); i.hasNext();)
      {
         UPnPDeviceIcon icon = (UPnPDeviceIcon)i.next();
         listElement.addChild(icon.convertToXmlElement(listElement));
      }
      
      // return root element
      return listElement;
   }
   
   /**
    * Converts this object from an XmlElement.
    *
    * @param element the XmlElement to convert from.
    * 
    * @return true if successful, false otherwise.
    */
   public boolean convertFromXmlElement(XmlElement element)   
   {
      boolean rval = true;
      
      // clear icon list
      clear();
      
      // convert icons
      for(Iterator i = element.getChildren("icon").iterator(); i.hasNext();)
      {
         XmlElement iconElement = (XmlElement)i.next();
         UPnPDeviceIcon icon = new UPnPDeviceIcon();
         if(icon.convertFromXmlElement(iconElement))
         {
            addIcon(icon);
         }
      }
      
      return rval;
   }
   
   /**
    * Adds a UPnPDeviceIcon to this list.
    * 
    * @param icon the UPnPDeviceIcon to add.
    */
   public void addIcon(UPnPDeviceIcon icon)
   {
      getIcons().add(icon);
   }
   
   /**
    * Removes a UPnPDeviceIcon from this list.
    * 
    * @param icon the UPnPDeviceIcon to remove.
    */
   public void removeIcon(UPnPDeviceIcon icon)
   {
      getIcons().remove(icon);
   }
   
   /**
    * Gets the UPnPDeviceIcons for this list in a vector.
    * 
    * @return the UPnPDeviceIcons for this list in a vector.
    */
   public Vector getIcons()
   {
      return mIcons;
   }
   
   /**
    * Clears the icons from this list.
    */
   public void clear()
   {
      getIcons().clear();
   }
   
   /**
    * Gets an iterator over the icons in this list.
    *
    * @return an iterator over the UPnPDeviceIcons in this list.
    */
   public Iterator iterator()
   {
      return getIcons().iterator();
   }
   
   /**
    * Gets the number of icons in this list.
    * 
    * @return the number of icons in this list.
    */
   public int getIconCount()
   {
      return getIcons().size();
   }
   
   /**
    * Gets the logger for this UPnPDeviceIconList.
    * 
    * @return the logger for this UPnPDeviceIconList.
    */
   public Logger getLogger()
   {
      return LoggerManager.getLogger("dbupnp");
   }
}
