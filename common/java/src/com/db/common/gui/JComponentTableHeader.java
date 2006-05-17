/*
 * Copyright (c) 2005-2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.common.gui;

import java.awt.Component;
import java.awt.Insets;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.UIManager;
import javax.swing.border.EtchedBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.TableColumnModelEvent;
import javax.swing.event.TableColumnModelListener;
import javax.swing.plaf.basic.BasicArrowButton;
import javax.swing.plaf.basic.BasicButtonUI;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumnModel;
import javax.swing.table.TableModel;

/**
 * A header for a JCompontentTable. Allows column sorting and header
 * highlighting. 
 * 
 * @author Dave Longley
 */
public class JComponentTableHeader extends JTableHeader
                                   implements MouseListener,
                                              MouseMotionListener
{
   /**
    * The JComponentHeaderRenderer.
    */
   protected JComponentHeaderRenderer mHeaderRenderer;
   
   /**
    * Creates a new JComponentTableHeader using the passed
    * table column model.
    * 
    * @param tcm the table column model to use.
    */
   public JComponentTableHeader(TableColumnModel tcm)
   {
      super(tcm);
      
      mHeaderRenderer = new JComponentHeaderRenderer();
      setDefaultRenderer(mHeaderRenderer);
      
      addMouseListener(this);
      addMouseMotionListener(this);
   }
   
   /** 
    * Sets the table associated with this header.
    *  
    * @param table the new table.
    */
   public void setTable(JTable table)
   {
      super.setTable(table);
      
      if(table != null)
      {
         // add header as column model listener
         table.getColumnModel().addColumnModelListener(mHeaderRenderer);
      }
   }
   
   /**
    * Invoked when the mouse button has been clicked (pressed
    * and released) on a component.
    * 
    * @param e the mouse event.
    */
   public void mouseClicked(MouseEvent e)
   {
      int column = columnAtPoint(e.getPoint());
      mHeaderRenderer.clickSortButton(column);
      repaint();
   }

   /**
    * Invoked when a mouse button has been pressed on a component.
    */
   public void mousePressed(MouseEvent e)
   {
      int column = columnAtPoint(e.getPoint());
      mHeaderRenderer.setMousePressedColumn(column);
      repaint();
   }

   /**
    * Invoked when a mouse button has been released on a component.
    * 
    * @param e the mouse event.
    */
   public void mouseReleased(MouseEvent e)
   {
      mHeaderRenderer.setMousePressedColumn(-1);
      repaint();
   }

   /**
    * Invoked when the mouse enters a component.
    * 
    * @param e the mouse event.
    */
   public void mouseEntered(MouseEvent e)
   {
      int column = columnAtPoint(e.getPoint());
      mHeaderRenderer.setMouseOverColumn(column);
      repaint();
   }

   /**
    * Invoked when the mouse exits a component.
    * 
    * @param e the mouse event.
    */
   public void mouseExited(MouseEvent e)
   {
      mHeaderRenderer.setMouseOverColumn(-1);
      repaint();
   }
   
   /**
    * Invoked when the mouse is being dragged.
    * 
    * @param e the mouse event.
    */
   public void mouseDragged(MouseEvent e)
   {
   }
   
   /**
    * Invoked when the mouse is being moved but no buttons are
    * pressed.
    * 
    * @param e the mouse event.
    */
   public void mouseMoved(MouseEvent e)
   {
      int column = columnAtPoint(e.getPoint());
      mHeaderRenderer.setMouseOverColumn(column);
      repaint();
   }
   
   /**
    * This class allows any JComponent to become a header renderer for a table
    * and adds a sort button to right of that component if the column
    * is currently sorted.
    * 
    * @author Dave Longley
    */
   public class JComponentHeaderRenderer implements TableCellRenderer,
                                                    TableColumnModelListener
   {
      /**
       * The sort button.
       */
      protected JButton mSortButton;
      
      /**
       * The sort ascending button.
       */
      protected JButton mSortAscendingButton;
      
      /**
       * The sort descending button.
       */
      protected JButton mSortDescendingButton;
      
      /**
       * The sorted column.
       */
      protected int mSortedColumn;
      
      /**
       * Whether the sorted column (is any) is sorted ascending or
       * descending.
       */
      protected boolean mSortAscending;
      
      /**
       * Mouse pressed column. The column that received a mouse press.
       */
      protected int mMousePressedColumn;
      
      /**
       * The mouse over column. The column that the mouse is over, if any.
       */
      protected int mMouseOverColumn;
      
      /**
       * Creates a new JComponentHeaderRenderer.
       */
      public JComponentHeaderRenderer()
      {
         // create the sort buttons
         createSortButtons();
         
         mSortedColumn = -1;
         mSortAscending = false;
         
         setMousePressedColumn(-1);
         setMouseOverColumn(-1);
      }
      
      /**
       * Creates the sort buttons (ascending/descending).
       */
      protected void createSortButtons()
      {
         // create sort ascending button
         mSortAscendingButton = new BasicArrowButton(BasicArrowButton.NORTH);
         mSortAscendingButton.setOpaque(false);
         mSortAscendingButton.setUI(new BasicButtonUI());
         mSortAscendingButton.setBorder(BorderFactory.createEmptyBorder());
         
         // create sort descending button
         mSortDescendingButton = new BasicArrowButton(BasicArrowButton.SOUTH);            
         mSortDescendingButton.setOpaque(false);
         mSortDescendingButton.setUI(new BasicButtonUI());
         mSortDescendingButton.setBorder(BorderFactory.createEmptyBorder());
      }
      
      /**
       * Sets the sort button.
       * 
       * @param ascending
       */
      protected void setSortButton()
      {
         mSortButton = (mSortAscending) ?
               mSortAscendingButton : mSortDescendingButton;
      }
      
      /**
       * Returns the component used for drawing the header. This method is
       * used to configure the renderer appropriately before drawing.
       *      
       * @param table this parameter is not used in this cell renderer.
       * @param value the value of the cell to be rendered.
       * @param isSelected true if the cell to be rendered is selected,
       *                   otherwise false.
       * @param hasFocus if true, render cell appropriately.
       * @param row the row index of the cell being drawn.
       * @param column the column index of the cell being drawn.
       *     
       * @return the component used for drawing the cell. 
       */
      public Component getTableCellRendererComponent(JTable table,
                                                     Object value,
                                                     boolean isSelected,
                                                     boolean hasFocus,
                                                     int row, int column)
      {
         JPanel headerPanel = new JPanel();
         int[][] plot = {{0,0},{1,0}};
         LayeredLayout ll = new LayeredLayout(plot);
         headerPanel.setLayout(ll);
         
         // set the sort button
         setSortButton();
         
         boolean pressed = (getMousePressedColumn() == column);
         mSortButton.getModel().setPressed(pressed);
         
         if(getMouseOverColumn() == column)
         {
            // if not pressed use lowered border, else use raised
            int type = (pressed) ? EtchedBorder.LOWERED : EtchedBorder.RAISED;
            headerPanel.setBorder(BorderFactory.createEtchedBorder(
                  type,
                  UIManager.getColor("controlHighlight"),
                  UIManager.getColor("controlShadow")));
         }
         else
         {
            headerPanel.setBorder(BorderFactory.createEtchedBorder(
                  EtchedBorder.LOWERED,
                  UIManager.getColor("controlHighlight"),
                  UIManager.getColor("controlDkShadow")));
         }
         
         JComponent jc = null;
         
         if(value instanceof JComponent)
         {
            jc = (JComponent)value;
         }
         else if(value != null)
         {
            JLabel label = new JLabel(value.toString());
            label.setOpaque(false);
            label.setHorizontalAlignment(JLabel.CENTER);
            //Font oldFont = label.getFont();
            //Font newFont = new Font(oldFont.getName(), Font.PLAIN,
                                    //oldFont.getSize());
            //label.setFont(newFont);
            jc = label;
         }
         
         // place the component
         ll.placeNext(jc, 0.0, 0.0, true, false, new Insets(2, 0, 2, 0));
         headerPanel.add(jc);
         
         // add the sort button if this is the sorted column
         if(mSortedColumn == column)
         {
            ll.placeNext(mSortButton, 0.0, 1.0, false, true);
            headerPanel.add(mSortButton);
         }
         
         return headerPanel;
      }
      
      /**
       * Gets the mouse pressed column.
       * 
       * @return the mouse pressed column.
       */
      public int getMousePressedColumn()
      {
         return mMousePressedColumn;
      }
      
      /**
       * Sets the mouse pressed column.
       * 
       * @param column the mouse pressed column.
       */
      public void setMousePressedColumn(int column)
      {
         mMousePressedColumn = column;
      }
      
      /**
       * Gets the mouse over column.
       * 
       * @return the mouse over column.
       */
      public int getMouseOverColumn()
      {
         return mMouseOverColumn;
      }
      
      /**
       * Sets the mouse over column.
       * 
       * @param column the mouse over column.
       */
      public void setMouseOverColumn(int column)
      {
         mMouseOverColumn = column;
      }      

      /**
       * Clicks the sort button on a particular column.
       * 
       * @param column the column to click the sort button on.
       */
      public void clickSortButton(int column)
      {
         if(column != -1)
         {
            // if this is a new column to sort, set sort to ascending
            // else invert it
            if(mSortedColumn != column)
            {
               mSortAscending = true;
               mSortedColumn = column;        
            }
            else
            {
               mSortAscending = !mSortAscending;
            }
            
            // get the table's model and sort it, if its sortable
            TableModel tm = table.getModel();
            if(tm instanceof SortableTableModel)
            {
               // get the model column index
               column =
                  table.getColumnModel().getColumn(column).getModelIndex();

               // sort the column
               SortableTableModel stm = (SortableTableModel)tm;
               stm.sortColumn(column, mSortAscending);
            }
         }
      }
      
      /**
       * Tells listeners that a column was added to the model.
       * 
       * @param e the column model event.
       */
      public void columnAdded(TableColumnModelEvent e)
      {
      }

      /**
       * Tells listeners that a column was removed from the model.
       * 
       * @param e the column model event.
       */
      public void columnRemoved(TableColumnModelEvent e)
      {
      }

      /**
       * Tells listeners that a column was repositioned.
       * 
       * @param e the column model event.
       */
      public void columnMoved(TableColumnModelEvent e)
      {
         // see if the column moved was the sorted one
         int fromIndex = e.getFromIndex();
         int toIndex = e.getToIndex();
         if(fromIndex != toIndex)
         {
            // move the sorted column
            if(fromIndex == mSortedColumn)
            {
               mSortedColumn = toIndex;
            }
            else if(toIndex == mSortedColumn)
            {
               mSortedColumn = fromIndex;
            }
            
            // move the mouseover column
            if(fromIndex == getMouseOverColumn())
            {
               setMouseOverColumn(toIndex);
            }
            else if(toIndex == getMouseOverColumn())
            {
               setMouseOverColumn(fromIndex);
            }
            
            // move the clicked column
            if(fromIndex == getMousePressedColumn())
            {
               setMousePressedColumn(toIndex);
            }
            else if(toIndex == getMousePressedColumn())
            {
               setMousePressedColumn(fromIndex);
            }
         }
      }

      /**
       * Tells listeners that a column was moved due to a margin change.
       * 
       * @param e the change event.
       */
      public void columnMarginChanged(ChangeEvent e)
      {
      }

      /**
       * Tells listeners that the selection model of the
       * TableColumnModel changed.
       * 
       * @param e the list selection event.
       */
      public void columnSelectionChanged(ListSelectionEvent e)
      {
      }
   }   
}
